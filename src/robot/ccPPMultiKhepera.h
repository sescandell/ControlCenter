#ifndef CC_PP_MULTI_KHEPERA_HEADER
#define CC_PP_MULTI_KHEPERA_HEADER

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include "ccProtocolProcessor.h"
#include "ccRobot.h"

namespace CCF
{
    class PPMultiKhepera: public ProtocolProcessor
    {
        Q_OBJECT

        public:
            PPMultiKhepera(const QString& _interfaceName);
            virtual ~PPMultiKhepera();
            
            void AddRobot(const Robot* robot);
            QByteArray GetData(const Robot* robot);
            QList<QByteArray> GetStatistics(const Robot* robot);
            void RemoveRobot(const Robot* robot);

        public slots:            

        signals:
            void startTimer();
            void stopTimer();

        protected:
            enum MachineStates { PPMK_MS_WAITING_NEW_COMMAND, PPMK_MS_WAITING_END_COMMAND };

            QList<QByteArray> extractData(const QByteArray& data);
            QString formatAddress(const QString& address) const;
            void formatData(QByteArray& data, const Robot* robot) const;
            void processData(const QByteArray& data);
            void processCommandData(QByteArray& data, int indexSeparator);
            void processStatisticsData(QByteArray& data);
            bool receptionIsComplete(const QByteArray& data) const;

            MachineStates   machineState;
            QByteArray      buffer;
            QMap<QString /*robot address*/, QList<QByteArray> > commands;
            QMap<QString /*robot address*/, QList<QList<QByteArray> > > statistics;
            QMutex          mutex;
            QString         lastAddress;
            QTimer          timerGetData;
            bool            timeOutGetData;
            


        protected slots:
            void on_timeoutGetData();

        private:

    }; // class PPMultiKhepera
}
#endif // #ifndef CC_PP_MULTI_KHEPERA_HEADER