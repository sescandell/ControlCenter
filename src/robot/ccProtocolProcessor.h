#ifndef CC_PROTOCOL_PROCESSOR_HEADER
#define CC_PROTOCOL_PROCESSOR_HEADER

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <qextserialport.h>

#include <QtDebug>

namespace CCF
{
    class Robot;

    class ProtocolProcessor: public QObject
    {
        Q_OBJECT

        public:
            ProtocolProcessor(const QString& _interfaceName);
            virtual ~ProtocolProcessor();
            
            bool Connect();
            bool Disconnect();
            void Flush();
            bool IsConnected();
            virtual QByteArray GetData(const Robot* robot) = 0;
            virtual QList<QByteArray> GetStatistics(const Robot* robot) = 0;
            void SendData(QByteArray data, const Robot* robot);
            

        public slots:               

        signals:

        protected:
            virtual void formatData(QByteArray& data, const Robot* robot) const = 0;
            virtual void processData(const QByteArray& data) = 0;
            virtual bool receptionIsComplete(const QByteArray& data) const = 0;

            
            QString interfaceName;
            QMutex mutexSerialPort;
            QextSerialPort serialPort;
            bool receptionComplete;

        protected slots: 
            void onReadyRead();
            

        private:

    }; // class ProtocolProcessor
}
#endif // #ifndef CC_PROTOCOL_PROCESSOR_HEADER