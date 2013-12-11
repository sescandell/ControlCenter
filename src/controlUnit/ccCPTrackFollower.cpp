#include "ccCPTrackFollower.h"

#include <QList>
#include <QPoint>

#include <cmath>

namespace CCF
{
    static const short int PARAM_ID_TRACK_FOLLOWER_MASK                       = 0x0F00;

    static const double    CC_CP_TRACK_FOLLOWER_PI                            = 3.14159265;
    static const double    CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE               = 2*CC_CP_TRACK_FOLLOWER_PI/180;
    static const double    CC_CP_TRACK_FOLLOWER_PRECISION_DESTINATION_REACHED = 0.25;
    static const double    CC_CP_TRACK_FOLLOWER_DISTANCE_DECREASING_SPEED     = 1.5;
    static const double    CC_CP_TRACK_FOLLOWER_FACTOR_DECREASING_SPEED       = 0.5;

    static int             robotsSize     = 10;

    CPTrackFollower::CPTrackFollower()
        :ControlProcessor()
    {}

    CPTrackFollower::~CPTrackFollower()
    {
        foreach(int robotId, robotsTracks.keys())
        {
            removeAssociation(robotId);
        } // foreach
    }

    QList<int> CPTrackFollower::GetParameter(short int parameterId, int specific)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        switch(parameterId & PARAM_ID_TRACK_FOLLOWER_MASK)
        {
            case PARAM_ID_TRACK_FOLLOWER_GET_ASSOCIATED_ROBOTS:
                return robotsTracks.keys(specific);
            default:
                return  QList<int>();
        };
    }

    bool CPTrackFollower::SetParameter(short int parameterId, const QMap<int, Robot*>* value)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        switch(parameterId & PARAM_ID_TRACK_FOLLOWER_MASK)
        {
            case PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_LIST:
                robots = value;                
                return true;
            default:
                return false;
        };
    }

    bool CPTrackFollower::SetParameter(short int parameterId, const QMap<int, Track*>* value)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        switch(parameterId & PARAM_ID_TRACK_FOLLOWER_MASK)
        {
            case PARAM_ID_TRACK_FOLLOWER_SET_TRACKS_LIST:
                tracks = value;
                return true;
            default:
                return false;
        };
    }

    bool CPTrackFollower::SetParameter(short int parameterId, QList<int>* value)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        switch(parameterId & PARAM_ID_TRACK_FOLLOWER_MASK)
        {
            case PARAM_ID_TRACK_FOLLOWER_ADD_ASSOCIATION:
                addAssociation(value->at(0),value->at(1),value->at(2));
                return true;
            case PARAM_ID_TRACK_FOLLOWER_SET_LAPS_TODO:
                setLapsTodo(value->at(0),value->at(1));
                return true;
            default:
                return false;
        };
    }

    bool CPTrackFollower::SetParameter(short parameterId, int value)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        switch(parameterId & PARAM_ID_TRACK_FOLLOWER_MASK)
        {
            case PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_SIZE:
                robotsSize = value;                
                return true;
            case PARAM_ID_TRACK_FOLLOWER_REMOVE_ASSOCIATION:
                removeAssociation(value);
                return true;
            case PARAM_ID_TRACK_FOLLOWER_START_FOLLOWER:
                startFollower(value);
                return true;
            case PARAM_ID_TRACK_FOLLOWER_STOP_FOLLOWER:
                stopFollower(value);
                return true;
            case PARAM_ID_TRACK_FOLLOWER_PAUSE_FOLLOWER:
                pauseFollower(value);
                return true;
            case PARAM_ID_TRACK_FOLLOWER_REMOVE_ALL_TRACK_ASSOCIATION:
                removeAllTrackAssociation(value);
                return true;
            
            default:
                return false;
        }; // switch
    } // SetParameter()

    void CPTrackFollower::RunProcess()
    {
        foreach(int robotId, robotsTracks.keys())
        {
            if(robotsStates.value(robotId).enable)
            {
                Robot* robot   = (*robots)[robotId];
                Track* track = (*tracks)[robotsTracks.value(robotId)];
                robot->lock();
                track->lock();
                // First we check if we have reached our destination
                // If we have, we update our destination
                if(destinationReached(robot,track))
                {
                    robot->Stop();
                    initializeNextDestination(robot,track);
                }
                else
                {
                    if(!robotOnGoodTrajectory(robot,track))
                    {
                        correctRobotTrajectory(robot,track);
                    } // !robotOnGoodTrajectory()
                    else
                    {
                        moveRobot(robot,track);
                    }                    
                }
                track->unlock();
                robot->unlock();
            } // if enable
        }// foreach robotsTracks
    } //  RunProcess()

    void CPTrackFollower::addAssociation(int robotId, int trackId, int lapsCount)
    {
        // Let's check that the robot and the track exist
        if(!robots->contains(robotId) || !tracks->contains(trackId)) return;

        TFState state;
        state.enable            = false;
        state.lapsToDo          = lapsCount;
        state.lapsDone          = 0;
        state.trackPointIndex   = 0;

        removeAssociation(robotId);
        robotsTracks.insert(robotId, trackId);
        robotsStates.insert(robotId, state);
    } // addAssociation()

    void CPTrackFollower::correctRobotTrajectory(Robot* robot, const Track* track) const
    {
        double delta = computeCorrectiveOrientation(robot,track);
        // If we want to use SetPositionToBeReach from Khepera class, we have to
        // Compute the value that left counter must reach to correct trajectory
        // Warning : this imply some modifications int his class:
        //   We can just check if the last command sent was the one we want to send.
        //   We have to check if the robot is moving or not (maybe it is block).
        //   Using SetPositionToBeReach, is much more complicated in fact.
        //
        //int leftCounterToReach;

        // Check if last order is not the same as the one we want to send.
        // If it is, we don't send our order.
        const QList<Robot::CommandInformation>& commandHistory = robot->GetCommandHistory();
        // If the list is empty, it means we never send anything to move the robot.
        // So we can directly send the command
        bool sendCommand = commandHistory.isEmpty();
        // If the last order was the same as the one we want to send ...
        // do not send it
        if(!sendCommand)
        {
            switch(commandHistory.first().movement)
            {
                case Robot::RM_MANUAL_ROTATE_LEFT:
                case Robot::RM_AUTOMATIC_ROTATE_LEFT:
                    sendCommand = delta<0;
                    break;
                case Robot::RM_MANUAL_ROTATE_RIGHT:
                case Robot::RM_AUTOMATIC_ROTATE_RIGHT:
                    sendCommand = delta>0;
                    break;
                default:
                    sendCommand = true;
                    break;
            };// switch
        }

        if(sendCommand)
        {
            robot->Stop();
            if(delta>0)
                robot->RotateLeft();
            else
                robot->RotateRight();
        }
    } // correctRobotTrajectory()

    bool CPTrackFollower::destinationReached(const Robot* robot, const Track* track) const
    {
        QPoint distance = track->at(robotsStates[robot->GetIdentifier()].trackPointIndex) - robot->GetPosition();
        return distance.manhattanLength()<CC_CP_TRACK_FOLLOWER_PRECISION_DESTINATION_REACHED*robotsSize;
    } // destinationReached()

    double CPTrackFollower::computeAngle(const Robot* robot, const Track* track) const
    //Compute the angle between robot orientation and nex destination in the interval [-PI;PI]
    {
        QPoint trajectory  = track->at(robotsStates[robot->GetIdentifier()].trackPointIndex) - robot->GetPosition();
        double theta       = atan2((double)(trajectory.y()),(double)(trajectory.x()));
        double orientation = robot->GetOrientation();

        
        double angle = orientation - theta;
        if(!(angle>=-CC_CP_TRACK_FOLLOWER_PI && angle<=CC_CP_TRACK_FOLLOWER_PI))
        {
            int sign = 1;
            if(angle <0)
                sign = -1;
            int quotient     = (sign*angle) / CC_CP_TRACK_FOLLOWER_PI;
            double remainder = fmod(sign*angle,CC_CP_TRACK_FOLLOWER_PI);
            if(quotient%2==0)
                angle = sign*remainder;
            else
                angle = sign*(remainder-CC_CP_TRACK_FOLLOWER_PI);
        }

        return angle;
    }


    double CPTrackFollower::computeCorrectiveOrientation(const Robot *robot, const Track *track) const
    {
        
        double angle = computeAngle(robot,track);

        if(angle>=-CC_CP_TRACK_FOLLOWER_PI/2 && angle<=CC_CP_TRACK_FOLLOWER_PI/2)
        {
            // Front is nearest than back
            return angle;
        }
        else
        {
            if(angle>=0) 
                return angle-CC_CP_TRACK_FOLLOWER_PI;
            else
                return angle+CC_CP_TRACK_FOLLOWER_PI;
        }
    }

    void CPTrackFollower::initializeNextDestination(const Robot* robot, const Track* track)
    {        
        // Go to the next point
        robotsStates[robot->GetIdentifier()].trackPointIndex++;
        while(robotsStates[robot->GetIdentifier()].trackPointIndex<track->size() && destinationReached(robot,track))
        {
            robotsStates[robot->GetIdentifier()].trackPointIndex++;
        }

        if(robotsStates[robot->GetIdentifier()].trackPointIndex >= track->size())        
        // We have reach the end of the track
        {
            robotsStates[robot->GetIdentifier()].lapsDone++;
            emit EndOfLapReached(robot->GetIdentifier());
            robotsStates[robot->GetIdentifier()].trackPointIndex=0;
            if(robotsStates[robot->GetIdentifier()].lapsToDo!=CC_CP_TRACK_FOLLOWER_INFINITE_LAPS && robotsStates[robot->GetIdentifier()].lapsDone>=robotsStates[robot->GetIdentifier()].lapsToDo)
            {
                robotsStates[robot->GetIdentifier()].enable=false;
                emit EndOfTrackReached(robot->GetIdentifier());
            }
        }
    }

    void CPTrackFollower::moveRobot(Robot* robot, const Track* track) const
        // Changelog :
        //  First implementation :
        //      Basic behavior : check if we have to go forward or backward, and send
        //      the order if needed ( = if different than the last order sent)
        //
        //  2010/06/24 :
        //      Take into account the distance to the destination :
        //      the more we are close to the target, the less the speed is important
    {
        const QList<Robot::CommandInformation>& commandHistory = robot->GetCommandHistory();
        int speed = robot->GetUserSpeed();

        // Identify if we have to go Forward or Backward
        double angle = computeAngle(robot,track);
        
        bool goForward = angle>=-CC_CP_TRACK_FOLLOWER_PI/2 && angle<=CC_CP_TRACK_FOLLOWER_PI/2;

        bool sendCommand = commandHistory.isEmpty();

        if(!sendCommand)
        {
            // If the last command sent is the same as the one we want to send
            // Do not send it
            sendCommand = (goForward && Robot::RM_FORWARD!=commandHistory.first().movement) || 
                            (!goForward && Robot::RM_BACKWARD!=commandHistory.first().movement);
        }

        // Change 2010/06/24
        //  Last command was the same, but check if speed was the same too
        if(!sendCommand)
        {
            speed = computeSpeed(robot, track);
            sendCommand = abs(speed) != abs(commandHistory.first().left);
        }

        if(sendCommand)
        {   
            robot->Stop();
            if(goForward)
                robot->GoForward(speed);
            else
                robot->GoBackward(speed);
        }
    }

    int CPTrackFollower::computeSpeed(const Robot* robot, const Track* track) const
    {
        QPoint distance = track->at(robotsStates[robot->GetIdentifier()].trackPointIndex) - robot->GetPosition();
        if( (double)distance.manhattanLength()<(double)CC_CP_TRACK_FOLLOWER_DISTANCE_DECREASING_SPEED*robotsSize )
            return robot->GetUserSpeed()*CC_CP_TRACK_FOLLOWER_FACTOR_DECREASING_SPEED+1; // "+1" is to be sure we do not ask the robot to not move
        else
            return robot->GetUserSpeed();
    }

    void CPTrackFollower::removeAssociation(int robotId)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        if(robotsTracks.contains(robotId))
        {
            (*robots)[robotId]->Stop();
            robotsTracks.remove(robotId);
            robotsStates.remove(robotId);
        }
    } // removeAssociation()

    bool CPTrackFollower::robotOnGoodTrajectory(const Robot* robot, const Track* track) const
    {
        double angle = computeAngle(robot,track);
        QPoint distance = track->at(robotsStates[robot->GetIdentifier()].trackPointIndex) - robot->GetPosition();

        switch(distance.manhattanLength()/robotsSize)
        {
            case 0:     // Normally never happen
                return true;
            case 1:     // We are very close
                //return abs(angle)<=6*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=6*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
            case 2:     // We are close
                return abs(angle)<=5*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=5*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
            case 3:     // We are close enough
                return abs(angle)<=4*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=4*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
            case 4:     // We are quite fare
                return abs(angle)<=3*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=3*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
            case 5:     // We are fare
                return abs(angle)<=2*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=2*CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
            default:    // We are very fare
                return abs(angle)<=CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE || abs(abs(angle)-CC_CP_TRACK_FOLLOWER_PI)<=CC_CP_TRACK_FOLLOWER_DELTA_MAX_ANGLE;
        }; // switch
    } // robotOnGoodTrajectory()

    void CPTrackFollower::startFollower(int robotId)
    {
        if(robotsTracks.contains(robotId))
        {
            robotsStates[robotId].enable = true;
        }
    } // startFollower()

    void CPTrackFollower::pauseFollower(int robotId)
    {
        if(robotsTracks.contains(robotId))
        {
            (*robots)[robotId]->lock();
            (*robots)[robotId]->Stop();
            robotsStates[robotId].enable = false;
            (*robots)[robotId]->unlock();

        }
    } // pauseFollower()

    void CPTrackFollower::stopFollower(int robotId)
    {
        pauseFollower(robotId);
        if(robotsTracks.contains(robotId))
        {
            robotsStates[robotId].lapsDone = 0;
            robotsStates[robotId].trackPointIndex = 0;
        }
    } // stopFollower()

    void CPTrackFollower::removeAllTrackAssociation(int trackId)
    {
        // It is up to the user to take care that
        // this function cannot be called while RunProcess()
        // is running
        foreach(int robotIdentifier, robotsTracks.keys())
        {
            if(robotsTracks.value(robotIdentifier)==trackId)
                removeAssociation(robotIdentifier);
        }
    } // removeAllTrackAssociation()

    void CPTrackFollower::setLapsTodo(int robotId, int value)
    {
        if(robotsTracks.contains(robotId))
        {
            robotsStates[robotId].lapsToDo = value;
        }
    } // setLapsTodo

} // namespace CCF