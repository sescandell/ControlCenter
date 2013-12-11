#include "ccPPSimpleKhepera.h"

namespace CCF
{
    static const char CC_PPS_END_OF_COMMAND   = '\n';
    static const char CC_PPS_END_OF_COMMAND_SECONDARY = '\r';



    PPSimpleKhepera::PPSimpleKhepera(const QString& _interfaceName)
        :ProtocolProcessor(_interfaceName)
    {
        machineState = PPS_MS_WAITING_NEW_COMMAND;
    }


    PPSimpleKhepera::~PPSimpleKhepera()
    {}

    void PPSimpleKhepera::formatData(QByteArray& /*data*/, const Robot* /*robot*/) const
    {
        // Nothing to do
        return;
    }


    QByteArray PPSimpleKhepera::GetData(const Robot* /*robot*/)
    {
        bool done = false;
        QByteArray command;
        while(!done)
        {
            mutex.lock();
            done = !lastCommand.isEmpty();
            if(done)
                command = lastCommand.takeFirst();
            mutex.unlock();
        }
        return command;
    }

    QList<QByteArray> PPSimpleKhepera::GetStatistics(const Robot* /*robot*/)
    {
        // Not implemented by default
        return QList<QByteArray>();
    }


    void PPSimpleKhepera::processData(const QByteArray& data)
    {
        QList<QByteArray> commands = extractData(data);
        mutex.lock();
        if(!commands.isEmpty())
        {
            lastCommand.append(commands);
        }
        receptionComplete = receptionIsComplete(data);
        mutex.unlock();
    }

    bool PPSimpleKhepera::receptionIsComplete(const QByteArray& data) const
    {
        static char lastCharacter = 0;

        if(data.isEmpty()) return false;
        char oldLastCharacter = lastCharacter;

        QByteArray t(data);
        t.replace(CC_PPS_END_OF_COMMAND_SECONDARY,CC_PPS_END_OF_COMMAND);

        lastCharacter = t[t.size()-1];
        if(t.size()==1)
        {
            if(lastCharacter==oldLastCharacter && lastCharacter==CC_PPS_END_OF_COMMAND)
            {
                lastCharacter = 0;
                return true;
            }
        }
        else
        {
            if(lastCharacter==CC_PPS_END_OF_COMMAND && lastCharacter==t[t.size()-2])
            {
                lastCharacter = 0;
                return true;
            }
        }

        return false;
    }

    

    QList<QByteArray> PPSimpleKhepera::extractData(const QByteArray& data)
    {
        QList<QByteArray> commands;
        QByteArray t(data);
        //qDebug() << "receiving:" << data.toHex();
        t.replace(CC_PPS_END_OF_COMMAND_SECONDARY,CC_PPS_END_OF_COMMAND);
        //qDebug() << "converted:" << t.toHex();
        QList<QByteArray> commandsList = t.split(CC_PPS_END_OF_COMMAND);
        // Remove empty ByteArray
        commandsList.removeAll(QByteArray());
        switch(machineState)
        {            
            case PPS_MS_WAITING_NEW_COMMAND: 
                if(commandsList.isEmpty()) break;
                if(CC_PPS_END_OF_COMMAND!=t[t.size()-1])
                {
                    buffer = commandsList.takeLast();
                    machineState = PPS_MS_WAITING_END_COMMAND;
                }       
                foreach(const QByteArray& command,commandsList)
                    commands.append(command);
                break;
            case PPS_MS_WAITING_END_COMMAND:
                if(!commandsList.isEmpty()) buffer += commandsList.takeFirst();
                if(commandsList.isEmpty())
                {
                    if(CC_PPS_END_OF_COMMAND==t[t.size()-1])
                    {
                        commands.append(buffer);
                        machineState = PPS_MS_WAITING_NEW_COMMAND;
                    }
                }    
                else
                {
                    commands.append(buffer);
                    if(CC_PPS_END_OF_COMMAND==t[t.size()-1])
                    {                        
                        machineState = PPS_MS_WAITING_NEW_COMMAND;
                    }
                    else
                    {
                        buffer = commandsList.takeLast();
                    }
                    foreach(const QByteArray& command,commandsList)
                            commands.append(command);
                }
                break;
            default:
                break;
        }; // switch(machineState)

        return commands;
    }
} // namespace CCF