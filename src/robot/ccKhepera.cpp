#include "ccKhepera.h"

#include <QVector>


namespace CCF
{       
    static const char CC_KHEPERA_COMMAND_END[]                          = "\r\n";
    static const char CC_KHEPERA_COMMAND_PARAMETERS_SEPARATOR           = ',';
    static const char CC_KHEPERA_COMMAND_ID_CONFIGURE_SPEED_PROFILE     = 'J';
    static const char CC_KHEPERA_COMMAND_ID_SET_POSITION_COUNTER        = 'G';
    static const char CC_KHEPERA_COMMAND_ID_GET_POSITION_COUNTER        = 'H';    
    static const char CC_KHEPERA_COMMAND_ID_SET_POSITION_TO_BE_REACHED  = 'C';
    static const char CC_KHEPERA_COMMAND_ID_SET_SPEED                   = 'D';
    static const char CC_KHEPERA_COMMAND_RESET[]                        = "restart\r\n";
    static const char CC_KHEPERA_END_MESSAGE_RESTART[]                  = "Serial Communication Protocol";

    static const char CC_KHEPERA_COMMAND_ID_GET_POSITION_COUNTER_REPONSE    = 'h';
    static const int  CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT                  = 10;

    static const int  CC_KHEPERA_INDEX_STATISTIC_RSSI   = 0;
    static const int  CC_KHEPERA_INDEX_STATISTIC_LQI    = 1;
    static const int  CC_KHEPERA_INDEX_STATISTIC_DELAY  = 2;

    const double Khepera::CC_KH_ROTATION_COEFFICIENT                        = -0.00305;

    static QByteArray generateCommand(const char commandId, const QVector<int> & params)
    {
        QByteArray command(&commandId);        
        foreach(int param, params)
        {
            command.append(CC_KHEPERA_COMMAND_PARAMETERS_SEPARATOR);
            command.append(QString::number(param));
        }
        command.append(CC_KHEPERA_COMMAND_END);
        return command;
    }

    Khepera::Khepera(const QString& _name, const QPoint& _position, Communicator* _communicator, const QString& _address)
        :Robot(_name,_position, _communicator, _address), 
         automaticSpeedLeft(CC_KHEPERA_MAX_SPEED_LEFT), automaticSpeedRight(CC_KHEPERA_MAX_SPEED_RIGHT)
    {        
        //Reset();
        // TODO : récupérer profil robot et enregistrer les bonnes valeurs de automaticSpeedLeft et Right et des accélarations
    }

    Khepera::~Khepera()
    {}


    void Khepera::ChangeOrientation(double rotation)
    {
        SetOrientation(orientation+CC_KH_ROTATION_COEFFICIENT*rotation);
    }

    QList<int> Khepera::GetPositionCounters()
    {
        QByteArray command = generateCommand(CC_KHEPERA_COMMAND_ID_GET_POSITION_COUNTER,QVector<int>());
        QByteArray data;
        int attempt = 0;
        do
        {
            sendCommand(command);
            data       = communicator->GetData(this);
            
        }while(data.isEmpty() && attempt++<CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT);
        QList<QByteArray>& element = data.split(CC_KHEPERA_COMMAND_PARAMETERS_SEPARATOR);
        processStatisticsData(communicator->GetStatistics(this));
        if(3==element.size() && CC_KHEPERA_COMMAND_ID_GET_POSITION_COUNTER_REPONSE==element.first()[0])
        {
            QList<int> counters;
            counters.append(element[1].toInt());
            counters.append(element[2].toInt());
            return counters;
        }
        else
            return QList<int>() << 0 << 0;
    }

    void Khepera::GoBackward(int speed)
    {
        SetSpeed(-speed,-speed);
    }

    void Khepera::GoForward(int speed)
    {
        SetSpeed(speed,speed);
    }

    void Khepera::Reset()
    {
        return;

        // Don't use this function ... data retrieving is not good
        sendCommand(QByteArray(CC_KHEPERA_COMMAND_RESET));
        //QByteArray message;
        //while(!message.contains(CC_KHEPERA_END_MESSAGE_RESTART))
        //{
        //    message += communicator->GetData(this);
        //}
    }

    void Khepera::ResetPositionCounters()
    {
        addCurrentPositionToHistory();
        int attempt = 0;
        QByteArray command = generateCommand(CC_KHEPERA_COMMAND_ID_SET_POSITION_COUNTER,QVector<int>(2,0));
        do
        {
            sendCommand(command);
        }while(communicator->GetData(this).isEmpty() && attempt++<CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT);
        
        processStatisticsData(communicator->GetStatistics(this));
        
        CommandInformation info;
        info.timestamp  = generateTimeStamp();
        info.movement   = RM_POSITION;
        info.left       = 0;
        info.right      = 0;

        addCommandToHistory(info);
    }

    void Khepera::RotateLeft(int speed)
    {
        SetSpeed(-speed,speed);
    }

    void Khepera::RotateRight(int speed)
    {
        SetSpeed(speed,-speed);
    }


    void Khepera::SetConfigurationSpeedProfile(int maxSpeedLeft, int accLeft, int maxSpeedRight, int accRight)
    {
        automaticSpeedLeft  = maxSpeedLeft;
        automaticSpeedRight = maxSpeedRight;

        QByteArray command = generateCommand(CC_KHEPERA_COMMAND_ID_CONFIGURE_SPEED_PROFILE,
                                    QVector<int>() << automaticSpeedLeft*CC_KHEPERA_MAX_SPEED_LEFT/100 << accLeft << automaticSpeedRight*CC_KHEPERA_MAX_SPEED_RIGHT/100 << accRight
                                    );
        int attempt = 0;
        do
        {
            sendCommand(command);
            
        }while(communicator->GetData(this).isEmpty() && attempt++<CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT);

        processStatisticsData(communicator->GetStatistics(this));
    }

    void Khepera::SetPositionToBeReached(int posLeft, int posRight)
    {        
        addCurrentPositionToHistory();
        int attempt = 0;
        QByteArray command = generateCommand(CC_KHEPERA_COMMAND_ID_SET_POSITION_TO_BE_REACHED,
                                    QVector<int>() << posLeft << posRight
                                    );
        do
        {
            sendCommand(command);
        }while(communicator->GetData(this).isEmpty() && attempt++<CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT);

        processStatisticsData(communicator->GetStatistics(this));

        CommandInformation info;
        info.timestamp  = generateTimeStamp();
        info.movement   = identifyMovement(posLeft,posRight);
        // There is better to do
        if(RM_MANUAL_ROTATE_LEFT==info.movement) info.movement = RM_AUTOMATIC_ROTATE_LEFT;
        else if(RM_MANUAL_ROTATE_RIGHT==info.movement) info.movement = RM_AUTOMATIC_ROTATE_RIGHT;
        info.left  = posLeft;
        info.right = posRight;
        addCommandToHistory(info);
    }

    void Khepera::SetSpeed(int speedLeft, int speedRight)
    {
        Khepera::CommandInformation info;
        info.movement   = identifyMovement(speedLeft,speedRight);

        if(info.movement!=RM_STOP)
            addCurrentPositionToHistory();

        int attempt = 0;
        QByteArray command = generateCommand(CC_KHEPERA_COMMAND_ID_SET_SPEED,
                        QVector<int>() << speedLeft*CC_KHEPERA_MAX_SPEED_LEFT/100 << speedRight*CC_KHEPERA_MAX_SPEED_RIGHT/100
                                    );
        do
        {
            sendCommand(command);
        }while(communicator->GetData(this).isEmpty() && attempt++<CC_KHEPERA_MAX_COMMUNICATION_ATTEMPT);
        
        processStatisticsData(communicator->GetStatistics(this));

        info.timestamp  = generateTimeStamp();        
        info.left       = speedLeft;
        info.right      = speedRight;

        if(info.movement==RM_STOP)
            addCurrentPositionToHistory();
        
        addCommandToHistory(info);
    }

    void Khepera::Stop()
    {
        SetSpeed(CC_KHEPERA_STOP_SPEED_LEFT,CC_KHEPERA_STOP_SPEED_RIGHT);
    }    

    void Khepera::addCurrentPositionToHistory()
    {
        CommandInformation info;
        info.timestamp  = generateTimeStamp();
        QList<int> counters = GetPositionCounters();
        info.movement   = RM_POSITION;
        info.left       = counters[0];
        info.right      = counters[1];

        addCommandToHistory(info);
    }    

    Robot::RobotMoves Khepera::identifyMovement(int speedLeft, int speedRight)
    // TODO : add a parameter in this function to know if we are using
    // a manual speed or a position to be reached command to be able to return
    // value KH_AUTOMATIC_ROTATE_XXXX
    {
        if(speedLeft==CC_KHEPERA_STOP_SPEED_LEFT && speedRight==CC_KHEPERA_STOP_SPEED_RIGHT)          return RM_STOP;
        else if(speedLeft > CC_KHEPERA_STOP_SPEED_LEFT && speedRight > CC_KHEPERA_STOP_SPEED_RIGHT)   return RM_FORWARD;
        else if(speedLeft < CC_KHEPERA_STOP_SPEED_LEFT && speedRight < CC_KHEPERA_STOP_SPEED_RIGHT)   return RM_BACKWARD;
        else if(speedLeft <= CC_KHEPERA_STOP_SPEED_LEFT && speedRight >= CC_KHEPERA_STOP_SPEED_RIGHT) return RM_MANUAL_ROTATE_LEFT;
        else if(speedLeft >= CC_KHEPERA_STOP_SPEED_LEFT && speedRight <= CC_KHEPERA_STOP_SPEED_RIGHT) return RM_MANUAL_ROTATE_RIGHT;
        else                                                                                          return RM_UNDEFINED; // never happened
    }

    void Khepera::processStatisticsData(QList<QByteArray>& statisticsData)
    {
        if(statisticsData.isEmpty()) return;

        StatisticsInformation info;
        info.timestamp = generateTimeStamp();
        info.RSSI      = statisticsData[CC_KHEPERA_INDEX_STATISTIC_RSSI].toInt();
        info.LQI       = statisticsData[CC_KHEPERA_INDEX_STATISTIC_LQI].toInt();
        info.Delay     = statisticsData[CC_KHEPERA_INDEX_STATISTIC_DELAY].toInt();

        addStatistic(info);
    }
}