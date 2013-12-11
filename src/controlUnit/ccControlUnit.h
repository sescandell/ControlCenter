#ifndef CC_CONTROL_UNIT_HEADER
#define CC_CONTROL_UNIT_HEADER

#include <QThread>
#include <QMap>
#include <QList>
#include <QSemaphore>

#include "ccControlProcessor.h"
#include "../image/ccImageProcessor.h"
#include "../robot/ccRobot.h"
#include "../track/ccTrack.h"

namespace CCF
{
    class ControlUnit: public QThread
    {
        Q_OBJECT

        public:
            ControlUnit(QSemaphore* _semSynchroImage);
            ~ControlUnit();

            QList<int> GetParameter(short int parameterId, int specific);

            bool SetParameter(short int parameterId, const QMap<int, Robot*>* value);
            bool SetParameter(short int parameterId, const QMap<int, Track*>* value);
            bool SetParameter(short int parameterId, QList<int>* value);
            bool SetParameter(short int parameterId, int value);

        signals:
            void NewPositionsAvailable(IPKListPositions);
            void EndOfLapReached(int /* Robot identifier */);
            void EndOfTrackReached(int /* Robot identifier */);

        protected:
            void run();

            ControlProcessor*   mainProcessor;
            QSemaphore*         semSynchroImage;
            QSemaphore          synchro;

        private:

    }; // class ControlUnit

} // namespace CCF

#endif // #ifndef CC_CONTROL_UNIT_HEADER