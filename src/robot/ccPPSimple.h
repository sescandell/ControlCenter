#ifndef CC_PP_SIMPLE_HEADER
#define CC_PP_SIMPLE_HEADER

#include <QList>
#include <QByteArray>
#include <QMutex>

#include "ccProtocolProcessor.h"

namespace CCF
{
    class PPSimple: public ProtocolProcessor
    {
        public:
            PPSimple(const QString& _interfaceName);
            virtual ~PPSimple();
            
            QByteArray GetData(const Robot* robot);            

        public slots:            

        signals:

        protected:
            enum MachineStates { PPS_MS_WAITING_NEW_COMMAND, PPS_MS_WAITING_END_COMMAND };

            QList<QByteArray> extractData(const QByteArray& data);
            void formatData(QByteArray& data, const Robot* robot);
            void processData(const QByteArray& data);

            MachineStates machineState;

            QByteArray buffer;
            QList<QByteArray> lastCommand;
            bool       newCommand;
            QMutex     mutex;
            


        protected slots:

        private:

    }; // class PPSimple
}
#endif // #ifndef CC_PP_SIMPLE_HEADER