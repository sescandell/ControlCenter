#include "ccCPTracker.h"
#include "ccCPFrontFinder.h"
#include "ccCPTrackFollower.h"

namespace CCF
{
    static const short int PARAM_ID_TRACKER_MASK    = 0x000F;
    
    static const int CC_CP_TRACKER_MAX_INT          = 0x0FFFFFFF;    
    static const int CC_CP_TRACKER_SYNCHRO_IP_PUT_COUNT_RESSOURCES = 1;
    static const int CC_CP_TRACKER_COUNT_SIZE_ROBOT_MAX_DISTANCE = 1;

    static int       robotSize     = 10;
    
    
    CPTracker::CPTracker(QSemaphore* _semSynchroImage)
        :ControlProcessor(), semSynchroImage(_semSynchroImage)
    {
        processors.append(new CPFrontFinder());
        CPTrackFollower* trackFollower = new CPTrackFollower();
        processors.append(trackFollower);
        
        connect(trackFollower, SIGNAL(EndOfTrackReached(int)), this, SIGNAL(EndOfTrackReached(int)), Qt::DirectConnection);
        connect(trackFollower, SIGNAL(EndOfLapReached(int)),   this, SIGNAL(EndOfLapReached(int)), Qt::DirectConnection);
    }

    CPTracker::~CPTracker()
    {
        lock();
        semSynchroImage->release(CC_CP_TRACKER_SYNCHRO_IP_PUT_COUNT_RESSOURCES);        
        foreach(ControlProcessor* processor, processors)
        {
            disconnect(processor,0 , this, 0);
            delete processor;
        }
        unlock();
    }

    void CPTracker::OnNewPositionsAvailable(IPKListPositions newPositions)
    {
        semSynchroImage->release(CC_CP_TRACKER_SYNCHRO_IP_PUT_COUNT_RESSOURCES);
        if(!newPositions.isEmpty())
        {
            positions = newPositions;
            RunProcess();
        }
    }

    QList<int> CPTracker::GetParameter(short int parameterId, int specific)
    {
        QList<int> value;
        value.clear();
        lock();
        foreach(ControlProcessor* processor, processors)
        {
            if(value.isEmpty())
                value = processor->GetParameter(parameterId,specific);
        }
        unlock();
        return value;
    }

    bool CPTracker::SetParameter(short int parameterId, const QMap<int, Robot*>* value)
    {
        switch(parameterId & PARAM_ID_TRACKER_MASK)
        {
            case PARAM_ID_TRACKER_SET_ROBOTS_LIST:
                lock();
                robots = value;
                unlock();
                return true;
            default:
                bool done = false;
                lock();
                foreach(ControlProcessor* processor, processors)
                {
                    done = done || processor->SetParameter(parameterId, value);
                }
                unlock();
                return done;
        };
    }

    bool CPTracker::SetParameter(short int parameterId, const QMap<int, Track*>* value)
    {
        bool done = false;
        lock();
        foreach(ControlProcessor* processor, processors)
        {
            done = done || processor->SetParameter(parameterId, value);
        }
        unlock();
        return done;
    }

    bool CPTracker::SetParameter(short int parameterId, QList<int>* value)
    {
        bool done = false;
        lock();
        foreach(ControlProcessor* processor, processors)
        {
            done = done || processor->SetParameter(parameterId, value);
        }
        unlock();
        return done;
    }

    bool CPTracker::SetParameter(short int parameterId, int value)
    {
        switch(parameterId & PARAM_ID_TRACKER_MASK)
        {
            case PARAM_ID_TRACKER_SET_ROBOTS_SIZE:
                lock();
                robotSize = (int)value;
                unlock();
                return true;
            default:
                bool done = false;
                lock();
                foreach(ControlProcessor* processor, processors)
                {
                    done = done || processor->SetParameter(parameterId, value);
                }
                unlock();
                return done;
        };
    }

    void CPTracker::RunProcess()
    {
        /////////////////////
        //  1- Analyze of the new robots positions
        //  2- Call all other treatment (first of them must be CPFrontFinder)
        lock();
        pointMatchingProcess();
        foreach(ControlProcessor* processor, processors)
            processor->RunProcess();
        unlock();
    } // RunProcess()

    void CPTracker::pointMatchingProcess()
    {
        if(robots && robots->size())
        {
            ////// Compute distance between the Khepera and the potential new positions
            QScopedArrayPointer<int> distances(new int[robots->size()*positions.size()]);
            int robotPosition = 0;
            int manhattanLength;
            QPoint difference;
            QList<int> listRobotId;
            foreach(Robot* robot, *robots)
            {                
                int positionNumber = 0;
                listRobotId << robot->GetIdentifier();
                foreach(QPoint position, positions)
                {           
                    robot->lock();
                    difference = position - robot->GetPosition();
                    robot->unlock();
                    manhattanLength = difference.manhattanLength();
                    distances[robotPosition*positions.size()+positionNumber] = (manhattanLength < CC_CP_TRACKER_COUNT_SIZE_ROBOT_MAX_DISTANCE*robotSize) ? manhattanLength : CC_CP_TRACKER_MAX_INT;
                    positionNumber++;
                }
                robotPosition++;
            }

            /////////////////////
            // Associate positions to a Khepera 
            int updated = 0;
            while(updated<robots->size() && updated<positions.size())
            {
                // Looking for the minimum distance
                int value = CC_CP_TRACKER_MAX_INT;
                int index = -1;
                for(int i=0; i<robots->size()*positions.size(); i++)
                {
                    if(distances[i]<value)
                    {
                        value    = distances[i];
                        index = i;
                    }
                }
                if(-1==index)
                {
                    // We do not succeed to match a circle to a Khepera.
                    // TODO :
                    // Foreach Khepera not updated, set position to interpolated position
                    //
                    // Let's out of the loop
                    break;
                }
                else
                {
                    // Update the Khepera position according to the circle found

                    Robot* robot = robots->value(listRobotId.at(index/positions.size()));
                    robot->lock();
                    robot->SetPosition(positions.at(index%positions.size()));
                    robot->unlock();
                    // Unvalidate the Khepera in the distance matrice
                    for(int i=index-index%positions.size(), j=0; j<positions.size(); j++)
                    {
                        distances[i+j] = CC_CP_TRACKER_MAX_INT;
                    }
                    // Unvalidate the circle in the distance matrice
                    for(int i=index%positions.size(), j=0; j<robots->size(); j++)
                    {
                        distances[j*positions.size()+i] = CC_CP_TRACKER_MAX_INT;
                    }
                    updated++;
                }
                
            } // while
        } // if(robots && robots->size())
    } // pointMatchingProcess()
} // namespace CCF