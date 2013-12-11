#include "ccProtocolProcessor.h"

namespace CCF
{
    static const BaudRateType   CC_PROTOCOL_PROCESSOR_SERIAL_PORT_BAUDRATE = BAUD9600;
    static const DataBitsType   CC_PROTOCOL_PROCESSOR_SERIAL_PORT_DATABITS = DATA_8;
    static const ParityType     CC_PROTOCOL_PROCESSOR_SERIAL_PORT_PARITY   = PAR_NONE;
    static const StopBitsType   CC_PROTOCOL_PROCESSOR_SERIAL_PORT_STOPBITS = STOP_2;

    ProtocolProcessor::ProtocolProcessor(const QString& _interfaceName)
        :QObject(),interfaceName(_interfaceName), receptionComplete(true)
    {        
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

        serialPort.setPortName( interfaceName );
        serialPort.setBaudRate( CC_PROTOCOL_PROCESSOR_SERIAL_PORT_BAUDRATE );
        serialPort.setDataBits( CC_PROTOCOL_PROCESSOR_SERIAL_PORT_DATABITS );
        serialPort.setParity(   CC_PROTOCOL_PROCESSOR_SERIAL_PORT_PARITY   );
        serialPort.setStopBits( CC_PROTOCOL_PROCESSOR_SERIAL_PORT_STOPBITS );
        
        Connect();
    }


    ProtocolProcessor::~ProtocolProcessor()
    {
        Disconnect();
    }

    bool ProtocolProcessor::Connect()
    {
        bool connected;
        mutexSerialPort.lock();
        connected = serialPort.open(QIODevice::ReadWrite);
        mutexSerialPort.unlock();
        return connected;
    }

    bool ProtocolProcessor::Disconnect()
    {
        if(IsConnected())
        {
            mutexSerialPort.lock();
            serialPort.close();
            mutexSerialPort.unlock();
        }

        return true;
    }

    void ProtocolProcessor::Flush()
    {
        mutexSerialPort.lock();
        serialPort.flush();
        mutexSerialPort.unlock();
    }

    bool ProtocolProcessor::IsConnected()
    {
        bool connected;
        mutexSerialPort.lock();
        // connected = serialPort.lineStatus() & LS_DSR
        connected = serialPort.isOpen();
        mutexSerialPort.unlock();

        return connected;
    }

    void ProtocolProcessor::SendData(QByteArray data, const Robot* robot)
    {
        bool done = false;
        formatData(data,robot);
        do
        {
            mutexSerialPort.lock();
            if(receptionComplete)
            {
                done = true;
                qDebug() << "envoie : " << data;
                serialPort.write(data);
            }
            mutexSerialPort.unlock();
        }while(!done);
    } 

    void ProtocolProcessor::onReadyRead()
    {
        mutexSerialPort.lock();
        int avail = serialPort.bytesAvailable();
        if( avail > 0 )
        {
            QByteArray data;
            data.resize(avail);
            int read = serialPort.read(data.data(), data.size());            
            if(  read > 0 ) 
            {
                qDebug() << "Recu : " << data;
                processData(data);
            }
        }
        mutexSerialPort.unlock();
    }

} // namespace CCF