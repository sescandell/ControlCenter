#include "ccRobot.h"
#include <cmath>
#include <QTime>

namespace CCF
{
    static const double CC_ROBOT_PI                     = 3.14159265;
    static const int CC_ROBOT_COMMAND_HISTORY_MAX_SIZE  = 100;
    static const int CC_ROBOT_STATISTICS_MAX_SIZE       = 100;

    Robot::Robot(const QString& _name, const QPoint& _position, Communicator* _communicator, const QString& _address)
        :Displayable(_name), QMutex(), position(_position), communicator(_communicator), address(_address),
         orientation(0), userSpeed(4), userRotationSpeed(2)
    {}

	Robot::~Robot()
	{}

    bool Robot::Connect()
    {
        return communicator->Connect();
    }

    bool Robot::Disconnect()
    {
        return communicator->Disconnect();
    }

    bool Robot::IsConnected()
    {
        return communicator->IsConnected();
    }

    const QString& Robot::GetAddress() const
    {
        return address;
    }

    const QList<Robot::CommandInformation>& Robot::GetCommandHistory() const
    {
        return commandHistory;
    }

    QList<Robot::StatisticsInformation> Robot::GetStatistics() const
    {
        return statistics;
    }

    QString Robot::GetInterfaceName() const
    {
        return communicator->GetInterfaceName();
    }

    double Robot::GetOrientation() const
    {
        return orientation;
    }


    QPoint Robot::GetPosition() const
    {
        return position;
    }

    int Robot::GetUserRotationSpeed() const
    {
        return userRotationSpeed;
    }

    int Robot::GetUserSpeed() const
    {
        return userSpeed;
    }

    void Robot::SetOrientation(double value)
    // Set the orientation in the interval [- PI; PI]
    {
        if(value>=-CC_ROBOT_PI && value<=CC_ROBOT_PI)
        {
            orientation = value;
        }
        else
        {
            int sign = 1;
            if(value <0)
                sign = -1;
            int quotient     = (sign*value) / CC_ROBOT_PI;
            double remainder = fmod(sign*value,CC_ROBOT_PI);
            if(quotient%2==0)
                orientation = sign*remainder;
            else
                orientation = sign*(remainder-CC_ROBOT_PI);
        }
        emit OrientationUpdated(identifier);
    }

    void Robot::SetPosition(const QPoint& newPosition)
    {
        position = newPosition;
        emit PositionUpdated(identifier);
    }

    void Robot::SetUserSpeed(int _userSpeed)
    {
        userSpeed = _userSpeed;
    }

    void Robot::SetUserRotationSpeed(int _userRotationSpeed)
    {
        userSpeed = _userRotationSpeed;
    }

    void Robot::addCommandToHistory(CommandInformation& info)
    {
        commandHistory.prepend(info);
        if(commandHistory.size()>CC_ROBOT_COMMAND_HISTORY_MAX_SIZE)
            commandHistory.removeLast();
    }

    void Robot::addStatistic(StatisticsInformation& info)
    {
        statistics.prepend(info);
        if(statistics.size()>CC_ROBOT_STATISTICS_MAX_SIZE)
            statistics.removeLast();
    }

    void Robot::sendCommand(QByteArray& command)
    {
        communicator->SendData(command,this);
    }

    int Robot::generateTimeStamp()
    {
        QTime t = QTime::currentTime();        
        return t.msec() + 1000*(t.second() + 60 * (t.minute() + 60 * t.hour()));
    }
}