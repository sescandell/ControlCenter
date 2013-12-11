#include "ccPPSimple.h"
#include <QDebug>

namespace CCF
{
    static const char CC_PPS_END_OF_COMMAND   = '\n';
    static const char CC_PPS_END_OF_COMMAND_SECONDARY = '\r';



    PPSimple::PPSimple(const QString& _interfaceName)
        :ProtocolProcessor(_interfaceName)
    {
        machineState = PPS_MS_WAITING_NEW_COMMAND;
        newCommand = false;
    }


    PPSimple::~PPSimple()
    {}

    void PPSimple::formatData(QByteArray& /*data*/, const Robot* /*robot*/)
    {
        // Nothing to do
        return;
    }


    QByteArray PPSimple::GetData(const Robot* /*robot*/)
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



    void PPSimple::processData(const QByteArray& data)
    {
        QList<QByteArray> commands = extractData(data);
        if(!commands.isEmpty())
        {
            mutex.lock();
            lastCommand.append(commands);
            mutex.unlock();
        }
    }

    

    QList<QByteArray> PPSimple::extractData(const QByteArray& data)
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