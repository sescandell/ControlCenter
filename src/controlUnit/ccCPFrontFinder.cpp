#include "ccCPFrontFinder.h"
#include <cmath>


namespace CCF
{
    static const short int PARAM_ID_FRONT_FINDER_MASK                       = 0x00F0;

    static const int CC_CP_FRONT_FINDER_MIN_SIZE_COMMAND_HISTORY            = 2;
    static const int CC_FRONT_FINDER_MAX_ATTEMPTS_ORIENTATION               = 3;

    static const double CC_CP_FRONT_FINDER_PI                               = 3.14159265;
    static const double CC_CP_FRONT_FINDER_COUNT_SIZE_ROBOT_MIN_DISTANCE    = 0.5;
    static const double CC_CP_FRONT_FINDER_MAX_DELTA_NEW_ORIENTATION        = 5*CC_CP_FRONT_FINDER_PI/180;

    static int       robotSize     = 10;

    CPFrontFinder::CPFrontFinder()
        :ControlProcessor()
    {}

    CPFrontFinder::~CPFrontFinder()
    {
        foreach(QPoint* p, oldPositions)
            delete p;
    }

    void CPFrontFinder::RunProcess()
    {
        foreach(Robot* robot, *robots)
        {
            robot->lock();
            if(!oldPositions.contains(robot->GetIdentifier()))
            // New robot
            {
                oldPositions.insert(robot->GetIdentifier(),new QPoint(robot->GetPosition()));
                oldTimestamps.insert(robot->GetIdentifier(),-1);
                oldCounters.insert(robot->GetIdentifier(),robot->GetPositionCounters().first());
                robotsNotInitialized.append(robot->GetIdentifier());
            }
            else
            {                                               
                const QList<Robot::CommandInformation>& commandHistory = robot->GetCommandHistory();
                if(commandHistory.size()<CC_CP_FRONT_FINDER_MIN_SIZE_COMMAND_HISTORY)
                {
                    robot->unlock();
                    break;
                }

                // Search for the last MOVEMENT different than POSITION, STOP or UNDEFINED
                // to process direction
                bool processed = false;
                foreach(const Robot::CommandInformation& info, commandHistory)
                {
                    switch(info.movement)
                    {
                        case Robot::RM_FORWARD:
                        case Robot::RM_BACKWARD:
                            processOrientationByLinearMovement(robot,info.movement);
                            // Except if the last command was a modification of the wheel position counters,
                            // last positions counters is in the second element (due to the organization of
                            // commandHistory).
                            if(Robot::RM_POSITION==commandHistory.first().movement)
                                // Modification of counters
                                oldCounters.insert(robot->GetIdentifier(),commandHistory.first().left);
                            else
                                // Movement
                                oldCounters.insert(robot->GetIdentifier(),commandHistory.at(1).left);
                            processed = true;
                            break;
                        case Robot::RM_AUTOMATIC_ROTATE_LEFT:
                        case Robot::RM_AUTOMATIC_ROTATE_RIGHT:
                        case Robot::RM_MANUAL_ROTATE_LEFT:
                        case Robot::RM_MANUAL_ROTATE_RIGHT:
                            processOrientationByRotation(robot);
                            processed = true;
                            break;
                        case Robot::RM_STOP:
                        case Robot::RM_POSITION:
                        case Robot::RM_UNDEFINED:
                        default:
                            break;
                    }; // switch
                    if(processed)
                    {
                        oldTimestamps.insert(robot->GetIdentifier(),commandHistory.first().timestamp);
                        break;
                    }
                } // Foreach commandHistory
            } // if !(new Robot)
            robot->unlock();
        } // foreach Robots
    } // RunProcess()

    bool CPFrontFinder::SetParameter(short int parameterId, int value)
    {
        switch(parameterId & PARAM_ID_FRONT_FINDER_MASK)
        {
            case PARAM_ID_FRONT_FINDER_SET_ROBOTS_SIZE:
                robotSize = value;                
                return true;
            default:
                return false;
        }; // switch
    } // SetParameter

    bool CPFrontFinder::SetParameter(short int parameterId, const QMap<int, Robot*>* value)
    {
        switch(parameterId & PARAM_ID_FRONT_FINDER_MASK)
        {
            case PARAM_ID_FRONT_FINDER_SET_ROBOTS_LIST:
                robots = value;                
                return true;
            default:
                return false;
        }; // switch
    } // SetParameter

    void CPFrontFinder::processOrientationByLinearMovement(Robot* robot, Robot::RobotMoves movement)
    {
        QPoint* oldPosition = oldPositions.value(robot->GetIdentifier());
        QPoint p = robot->GetPosition() - *oldPosition;
        if(p.manhattanLength()<CC_CP_FRONT_FINDER_COUNT_SIZE_ROBOT_MIN_DISTANCE*robotSize) return;
        //if(p.y()==0 && p.x()==0) return;
        int sign = 1;        
        switch(movement)
        {                
            // Normal cases
            case Robot::RM_FORWARD:
                sign = 1;
                break;
            case Robot::RM_BACKWARD:
                sign = -1;
                break;            
            // Skip the following cases
            case Robot::RM_AUTOMATIC_ROTATE_LEFT:     //
            case Robot::RM_AUTOMATIC_ROTATE_RIGHT:    //
            case Robot::RM_MANUAL_ROTATE_LEFT:        // Normally never happened
            case Robot::RM_MANUAL_ROTATE_RIGHT:       // 
            case Robot::RM_UNDEFINED:                 //
            case Robot::RM_STOP:                      //
            case Robot::RM_POSITION:                  //
            default:                                  //
                return;
        }; // switch
        double newOrientation = atan2((double)(sign*p.y()),(double)(sign*p.x()));
        if(robotsNotInitialized.contains(robot->GetIdentifier()) || abs(abs(newOrientation)-abs(robot->GetOrientation()))<=CC_CP_FRONT_FINDER_MAX_DELTA_NEW_ORIENTATION)
        {
            robotsNotInitialized.removeAll(robot->GetIdentifier());
            robot->SetOrientation(newOrientation);
            deniedOrientation[robot->GetIdentifier()] = 0;
        }
        else
        {            
            // We just check that it is not a long time we want to assign a new value to the robot
            // that is very different than the current orientation.
            // If it is the case, we have to envisage that current orientation
            // is not the good one.
            if(++deniedOrientation[robot->GetIdentifier()]==CC_FRONT_FINDER_MAX_ATTEMPTS_ORIENTATION)
            {
                deniedOrientation[robot->GetIdentifier()] = 0;
                robot->SetOrientation(newOrientation);
            }
        }

        *oldPosition = robot->GetPosition();
    } // processOrientationByLinearMovement

    void CPFrontFinder::processOrientationByRotation(Robot* robot)
    {
        // In this algorithm, we assume that in a rotation movement:
        // left = -right
        
        // Initialization: we determine the left wheel end position
        QListIterator<Robot::CommandInformation> i(robot->GetCommandHistory());
        //if(!i.hasNext()) return; // Useless because if we are here, that means we have succeed the test line 45 of this file
        int oldTimestamp = oldTimestamps.value(robot->GetIdentifier());
        int oldCounter   = oldCounters.value(robot->GetIdentifier());
        int lastValue    = 0;
        int rotation     = 0;        
        switch(i.peekNext().movement)
        {
            int counter;
            case Robot::RM_STOP:            
                if(oldTimestamp>=i.next().timestamp) return;
                // First element is KH_STOP, so second element is KH_POSITION
                // and contains the latest position of wheels
                //if(!i.hasNext()) return; // Useless because if we are here, that means we have succeed the test line 45 of this file
                lastValue = i.next().left;
                oldCounters.insert(robot->GetIdentifier(), lastValue);                
                break;
            case Robot::RM_AUTOMATIC_ROTATE_LEFT:
            case Robot::RM_AUTOMATIC_ROTATE_RIGHT:
            case Robot::RM_MANUAL_ROTATE_LEFT:
            case Robot::RM_MANUAL_ROTATE_RIGHT:
                counter = robot->GetPositionCounters().first();
                if(oldTimestamp==i.next().timestamp)
                // It is not the first time we use this information
                // we have to process it differently
                {
                    rotation += counter - oldCounters.value(robot->GetIdentifier());
                    lastValue = oldCounter;
                }
                else
                // It is the first time we use this information
                {
                    //if(!i.hasNext()) return; // Useless because if we are here, that means we have succeed the test line 45 of this file
                    lastValue = i.next().left;
                    rotation += counter - lastValue;                
                }
                oldCounters.insert(robot->GetIdentifier(), counter);
                break;
            case Robot::RM_POSITION:      // Special case
                counter = robot->GetPositionCounters().first();
                if(oldTimestamp>=i.peekNext().timestamp)
                {                    
                    rotation += counter - oldCounters.value(robot->GetIdentifier());
                    lastValue = oldCounter;
                }
                else
                {
                    lastValue = i.next().left;        // New left counter value sent to the robot
                    rotation += counter - lastValue;
                    lastValue = i.next().left;        // Robot left counter before the command had been sent                    
                }
                oldCounters.insert(robot->GetIdentifier(), counter);
                break;
            case Robot::RM_FORWARD:       //
            case Robot::RM_BACKWARD:      // Normally never happened
                //oldCounter = 0;
            case Robot::RM_UNDEFINED:     //
            default:
                return;
        }; // switch

        // Now, we go through the history until the oldest rotation younger than oldTimestamp
        // and compute by the way the deviation        
        while(i.hasNext() && i.peekNext().timestamp>oldTimestamp)
        {
            switch(i.next().movement)
            {
                case Robot::RM_AUTOMATIC_ROTATE_LEFT:     //
                case Robot::RM_AUTOMATIC_ROTATE_RIGHT:    // Interesting cases
                case Robot::RM_MANUAL_ROTATE_LEFT:        //
                case Robot::RM_MANUAL_ROTATE_RIGHT:       //
                    if(i.hasNext())
                    {
                        rotation += lastValue - i.peekNext().left;
                        lastValue = i.next().left;
                    }
                    break;
                case Robot::RM_POSITION:      // Special case
                    // TODO
                    break;
                case Robot::RM_FORWARD:       //
                case Robot::RM_BACKWARD:      //
                case Robot::RM_STOP:
                    if(i.hasNext())
                    {
                        lastValue = i.next().left;          // There is no rotation
                    }
                    break;
                case Robot::RM_UNDEFINED:     //
                default:                        // Normally never happened
                    break;
            }; // switch
        } // while

        if(i.hasNext())
        {
            switch(i.next().movement)
            {
                case Robot::RM_AUTOMATIC_ROTATE_LEFT:     //
                case Robot::RM_AUTOMATIC_ROTATE_RIGHT:    // Interesting cases
                case Robot::RM_MANUAL_ROTATE_LEFT:        //
                case Robot::RM_MANUAL_ROTATE_RIGHT:       //
                    rotation += lastValue - oldCounter;
                    break;
                case Robot::RM_FORWARD:
                case Robot::RM_BACKWARD:
                case Robot::RM_STOP:
                case Robot::RM_POSITION:
                case Robot::RM_UNDEFINED:
                default:
                    break;

            }; // switch
        }
        // Now we can apply the formula to compute the new orientation
        // The negative sign is here because, we compute the rotation (in radians) based on the left wheel.
        // In fact, when the left wheel is positive, the robot rotate in counterclockwise
        robot->ChangeOrientation(-rotation);
    } // processOrientationByRotation

} // namespace