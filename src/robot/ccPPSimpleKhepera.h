#ifndef CC_PP_SIMPLE_HEADER
#define CC_PP_SIMPLE_HEADER

#include <QList>
#include <QByteArray>
#include <QMutex>

#include "ccProtocolProcessor.h"
#include "ccRobot.h"


namespace CCF
{
    class PPSimpleKhepera: public ProtocolProcessor
    {
        public:
            PPSimpleKhepera(const QString& _interfaceName);
            virtual ~PPSimpleKhepera();
            
            QByteArray GetData(const Robot* robot);
            QList<QByteArray> GetStatistics(const Robot* robot);

        public slots:            

        signals:

        protected:
            enum MachineStates { PPS_MS_WAITING_NEW_COMMAND, PPS_MS_WAITING_END_COMMAND };

            QList<QByteArray> extractData(const QByteArray& data);
            void formatData(QByteArray& data, const Robot* robot) const;
            void processData(const QByteArray& data);
            bool receptionIsComplete(const QByteArray& data) const;

            MachineStates machineState;

            QByteArray buffer;
            QList<QByteArray> lastCommand;
            QMutex     mutex;
            


        protected slots:

        private:

    }; // class PPSimpleKhepera
}
#endif // #ifndef CC_PP_SIMPLE_HEADER