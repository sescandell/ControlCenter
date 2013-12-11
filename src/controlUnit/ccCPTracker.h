#ifndef CC_CP_TRACKER_HEADER
#define CC_CP_TRACKER_HEADER

#include <QList>
#include <QMap>
#include <QSemaphore>
#include "ccControlProcessor.h"
#include "../image/ccImageProcessor.h" // IPKListPositions

namespace CCF
{
    class CPTracker: public ControlProcessor, protected QMutex
    {        
        
        Q_OBJECT

        public:               
            static const short int PARAM_ID_TRACKER_SET_ROBOTS_LIST = 0x0001;
            static const short int PARAM_ID_TRACKER_SET_ROBOTS_SIZE = 0x0002;

            CPTracker(QSemaphore* _semSynchroImage);
            virtual ~CPTracker();

            void RunProcess();

            QList<int> GetParameter(short int parameterId, int specific);

            bool SetParameter(short int parameterId, const QMap<int, Robot*>* value);
            bool SetParameter(short int parameterId, const QMap<int, Track*>* value);
            bool SetParameter(short int parameterId, QList<int>* value);
            bool SetParameter(short int parameterId, int value);
            

        public slots:
            void OnNewPositionsAvailable(IPKListPositions);

        signals:
            void EndOfLapReached(int /* Robot identifier */);
            void EndOfTrackReached(int /* Robot identifier */);
            

        protected:
            void pointMatchingProcess();

            IPKListPositions            positions;
            QList<ControlProcessor*>    processors;
            const QMap<int, Robot*>*    robots;
            QSemaphore*                 semSynchroImage;            

        private:

    }; // class CPTracker

} // namespace CCF

#endif // #ifndef CC_CP_TRACKER_HEADER