#ifndef CC_CONTROL_PROCESSOR_HEADER
#define CC_CONTROL_PROCESSOR_HEADER

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMap>
#include <QtCore/QList>

#include "../robot/ccRobot.h"
#include "../track/ccTrack.h"

namespace CCF
{
    class ControlProcessor: public QObject
    {
        Q_OBJECT

        public:            
            virtual ~ControlProcessor();

            virtual void RunProcess() = 0;

            virtual QList<int> GetParameter(short int parameterId, int specific);

            virtual bool SetParameter(short int parameterId, const QMap<int, Robot*>* value);
            virtual bool SetParameter(short int parameterId, const QMap<int, Track*>* value);
            virtual bool SetParameter(short int parameterId, QList<int>* value);
            virtual bool SetParameter(short int parameterId, int value);
            

        protected:
            ControlProcessor();

        private:

    }; // class ControlProcessor

} // namespace CCF

#endif // #ifndef CC_CONTROL_PROCESSOR_HEADER