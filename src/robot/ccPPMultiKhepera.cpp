#include "ccPPMultiKhepera.h"


#include <QtCore/QRegExp>

namespace CCF
{
    static const char CC_PPMK_FORMAT_SEPARATOR          = '_';
    static const int  CC_PPMK_SIZE_OF_ADDRESS           = 2;
    static const char CC_PPMK_START_OF_ADDRESS          = '0';
    static const char CC_PPMK_END_OF_COMMAND            = '\n';
    static const char CC_PPMK_END_OF_COMMAND_SECONDARY  = '\r';

    static const QString CC_PPMK_REGEXP_RSSI            = "RSSI=(\\-\\d+)";
    static const QString CC_PPMK_REGEXP_LQI             = "LQI=(\\d+)";
    static const QString CC_PPMK_REGEXP_DELAY           = "DELAY=(\\d+)";

    static const int CC_PPMK_STATISTICS_MAX_SIZE        = 100;

    static const int CC_PMK_TIMEOUT_INTERVAL            = 100; //msec



    PPMultiKhepera::PPMultiKhepera(const QString& _interfaceName)
        :ProtocolProcessor(_interfaceName),lastAddress(QString()), timeOutGetData(false)
    {
        machineState = PPMK_MS_WAITING_NEW_COMMAND;

        timerGetData.setInterval(CC_PMK_TIMEOUT_INTERVAL);
        timerGetData.setSingleShot(true);
        connect(&timerGetData,  SIGNAL(timeout()),      this,           SLOT(on_timeoutGetData()));
        connect(this,           SIGNAL(startTimer()),   &timerGetData,  SLOT(start()));
        connect(this,           SIGNAL(stopTimer()),    &timerGetData,  SLOT(stop()));
    }


    PPMultiKhepera::~PPMultiKhepera()
    {}

    QString PPMultiKhepera::formatAddress(const QString& address) const
    {
        if(address.size()<CC_PPMK_SIZE_OF_ADDRESS)
            return QString(CC_PPMK_START_OF_ADDRESS)+address;
        else
            return address;
    }

    void PPMultiKhepera::formatData(QByteArray& data, const Robot* robot) const
    {
        QByteArray identifier;
        identifier.append(formatAddress(robot->GetAddress()));
        identifier.append(CC_PPMK_FORMAT_SEPARATOR);
        data.prepend(identifier);
    }

    void PPMultiKhepera::AddRobot(const Robot *robot)
    {
        mutex.lock();
        QString address = formatAddress(robot->GetAddress());
        commands.insert(address, QList<QByteArray>());
        statistics.insert(address, QList<QList<QByteArray> >());
        mutex.unlock();
    }

    void PPMultiKhepera::RemoveRobot(const Robot* robot)
    {
        mutex.lock();
        commands.remove(formatAddress(robot->GetAddress()));
        mutex.unlock();
    }


    QByteArray PPMultiKhepera::GetData(const Robot* robot)
    {
        bool done;
        QByteArray command;
        command.clear();
        QString address = formatAddress(robot->GetAddress());
        timeOutGetData = false;
        emit startTimer();
        do
        {
            mutex.lock();
            done = !commands[address].isEmpty();
            if(done)
            {
                emit stopTimer();
                command = commands[formatAddress(robot->GetAddress())].takeFirst();
            }
            mutex.unlock();
        } while(!done && !timeOutGetData);
        return command;
    }

    void PPMultiKhepera::on_timeoutGetData()
    {
        mutex.lock();
        timeOutGetData = true;
        mutex.unlock();
    }

    QList<QByteArray> PPMultiKhepera::GetStatistics(const Robot* robot)
    {
        mutex.lock();
        if(statistics.contains(formatAddress(robot->GetAddress())) && !(statistics[formatAddress(robot->GetAddress())].isEmpty()))
        {
            
            QList<QByteArray> stats = statistics[formatAddress(robot->GetAddress())].takeFirst();
            mutex.unlock();
            return stats;
        }
        mutex.unlock();
        return QList<QByteArray>();
    }



    void PPMultiKhepera::processData(const QByteArray& data)
    {
        QList<QByteArray> commandsList = extractData(data);
        mutex.lock();
        if(!commandsList.isEmpty())
        {
            for(int index=0;index<commandsList.size();index++)
            {
                QByteArray& command = commandsList[index];
                int indexOfSeparator = command.indexOf(CC_PPMK_FORMAT_SEPARATOR);
                if(indexOfSeparator==-1)
                {
                    // Statistic data
                    processStatisticsData(command);
                }
                else
                {
                    // response comnmand
                    processCommandData(command,indexOfSeparator);
                }
            } // for
        }
        receptionComplete = receptionIsComplete(data);
        mutex.unlock();
    }

    bool PPMultiKhepera::receptionIsComplete(const QByteArray& data) const
    {
        static char lastCharacter = 0;

        if(data.isEmpty()) return false;
        char oldLastCharacter = lastCharacter;

        QByteArray t(data);
        t.replace(CC_PPMK_END_OF_COMMAND_SECONDARY,CC_PPMK_END_OF_COMMAND);

        lastCharacter = t[t.size()-1];
        if(t.size()==1)
        {
            if(lastCharacter==oldLastCharacter && lastCharacter==CC_PPMK_END_OF_COMMAND)
            {
                lastCharacter = 0;
                return true;
            }
        }
        else
        {
            if(lastCharacter==CC_PPMK_END_OF_COMMAND && lastCharacter==t[t.size()-2])
            {
                lastCharacter  = 0;
                return true;
            }
        }

        return false;
    }

    void PPMultiKhepera::processCommandData(QByteArray& data, int indexSeparator)
    {
        QString address(data.left(indexSeparator));
        data.remove(0,indexSeparator+1);
        if(commands.contains(address))
        {
            lastAddress = address;
            commands[address].append(data);
        }
    }

    void PPMultiKhepera::processStatisticsData(QByteArray& data)
    {
        if(lastAddress.isEmpty()) return;
        
        QByteArray rssi(QByteArray::number(-1));
        QByteArray lqi(QByteArray::number(-1));
        QByteArray delay(QByteArray::number(-1));

        QRegExp regExp(CC_PPMK_REGEXP_RSSI);
        
        int index = regExp.indexIn(data,0);
        if(index!=-1)
            rssi =  regExp.cap(1).toAscii();

        regExp.setPattern(CC_PPMK_REGEXP_LQI);
        index = regExp.indexIn(data,0);
        if(index!=-1)
            lqi = regExp.cap(1).toAscii();

        regExp.setPattern(CC_PPMK_REGEXP_DELAY);
        index = regExp.indexIn(data,0);
        if(index!=-1)
            delay = regExp.cap(1).toAscii();

        QList<QByteArray> temp;
        temp << rssi << lqi << delay;

        statistics[lastAddress].append(temp);
        if(statistics[lastAddress].size()>CC_PPMK_STATISTICS_MAX_SIZE)
            statistics[lastAddress].removeFirst();
    }

    

    QList<QByteArray> PPMultiKhepera::extractData(const QByteArray& data)
    {
        QList<QByteArray> commandsReturn;
        QByteArray t(data);
        t.replace(CC_PPMK_END_OF_COMMAND_SECONDARY,CC_PPMK_END_OF_COMMAND);
        QList<QByteArray> commandsList = t.split(CC_PPMK_END_OF_COMMAND);
        // Remove empty ByteArray
        commandsList.removeAll(QByteArray());
        switch(machineState)
        {            
            case PPMK_MS_WAITING_NEW_COMMAND: 
                if(commandsList.isEmpty()) break;
                if(CC_PPMK_END_OF_COMMAND!=t[t.size()-1])
                {
                    buffer = commandsList.takeLast();
                    machineState = PPMK_MS_WAITING_END_COMMAND;
                }
                foreach(const QByteArray& command,commandsList)
                    commandsReturn.append(command);
                break;
            case PPMK_MS_WAITING_END_COMMAND:
                if(!commandsList.isEmpty()) buffer += commandsList.takeFirst();
                if(commandsList.isEmpty())
                {
                    if(CC_PPMK_END_OF_COMMAND==t[t.size()-1])
                    {
                        commandsReturn.append(buffer);
                        machineState = PPMK_MS_WAITING_NEW_COMMAND;
                    }
                }    
                else
                {
                    commandsReturn.append(buffer);
                    if(CC_PPMK_END_OF_COMMAND==t[t.size()-1])
                    {                        
                        machineState = PPMK_MS_WAITING_NEW_COMMAND;
                    }
                    else
                    {
                        buffer = commandsList.takeLast();
                    }
                    foreach(const QByteArray& command,commandsList)
                            commandsReturn.append(command);
                }
                break;
            default:
                break;
        }; // switch(machineState)

        return commandsReturn;
    }
} // namespace CCF