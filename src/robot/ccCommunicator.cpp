#include "ccCommunicator.h"

#include "ccPPSimpleKhepera.h"
#include "ccPPMultiKhepera.h"

namespace CCF
{
    Communicator::Communicator(const QString& _interfaceName, Mode _mode)
        :QThread(), interfaceName(_interfaceName), mode(_mode), protocolProcessor(0)
    {        
        start();
        semReady.acquire();
    }

    Communicator::~Communicator()
    {
        quit();
        wait();
        Disconnect();
        delete protocolProcessor;
    }

    bool Communicator::AddRobot(const Robot* robot)
    {
        if(COMMUNICATOR_MODE_MULTI_ROBOT!=mode) return false;
        
        ((PPMultiKhepera*)protocolProcessor)->AddRobot(robot);
        return true;
    }

    Communicator::Mode Communicator::GetMode() const
    {
        return mode;
    }

    void Communicator::run()
    {
        // Initialization
        switch(mode)
        {
            case COMMUNICATOR_MODE_MULTI_ROBOT:
                protocolProcessor = new PPMultiKhepera(interfaceName);
                break;
            case COMMUNICATOR_MODE_SINGLE_ROBOT:
            default:
                protocolProcessor = new PPSimpleKhepera(interfaceName);
                break;

        };
        // Inform the main thread that the communication is ready
        semReady.release(1);

        // Running
        exec();
    }



    bool Communicator::Connect()
    {
        return protocolProcessor->Connect();
    }

    bool Communicator::Disconnect()
    {
        return protocolProcessor->Disconnect();
    }


    QByteArray Communicator::GetData(const Robot *robot)
    {
        return protocolProcessor->GetData(robot);
    }

    QList<QByteArray> Communicator::GetStatistics(const Robot *robot)
    {
        return protocolProcessor->GetStatistics(robot);
    }

    void Communicator::SendData(QByteArray& data, const Robot* robot)
    {
        //protocolProcessor->Flush();
        protocolProcessor->SendData(data, robot);
    }

    bool Communicator::IsConnected()
    {
        return protocolProcessor->IsConnected();
    }

    const QString& Communicator::GetInterfaceName() const
    {
        return interfaceName;
    }

    bool Communicator::RemoveRobot(const Robot* robot)
    {
        if(COMMUNICATOR_MODE_MULTI_ROBOT!=mode) return false;
        
        ((PPMultiKhepera*)protocolProcessor)->RemoveRobot(robot);
        return true;
    }
} // namespace CCF