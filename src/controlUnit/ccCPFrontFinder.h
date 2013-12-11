#ifndef CC_CP_FRONT_FINDER_HEADER
#define CC_CP_FRONT_FINDER_HEADER

#include <QMap>
#include <QPoint>

#include "ccControlProcessor.h"
#include "../robot/ccRobot.h"

namespace CCF
{
    class CPFrontFinder: public ControlProcessor
    {
        Q_OBJECT

        public:            
            static const short int PARAM_ID_FRONT_FINDER_SET_ROBOTS_LIST = 0x0010;
            static const short int PARAM_ID_FRONT_FINDER_SET_ROBOTS_SIZE = 0x0020;

            CPFrontFinder();
            ~CPFrontFinder();
            
            void RunProcess();
            bool SetParameter(short int parameterId, const QMap<int, Robot*>* value);
            bool SetParameter(short int parameterId, int value);

        protected:
            void processOrientationByLinearMovement(Robot* robot, Robot::RobotMoves movement);
            void processOrientationByRotation(Robot* robot);

            QMap<int/* robot identifier*/, int>             deniedOrientation;
            QMap<int/* robot identifier*/, int>             oldCounters;
            QMap<int/* robot identifier*/, QPoint*>         oldPositions;
            QMap<int/* robot identifier*/, int>             oldTimestamps;
            const QMap<int/* robot identifier*/, Robot*>*   robots;
            QList<int/* robot identifier*/>                 robotsNotInitialized;


        private:
    };
} // namespace CCF
#endif // #ifndef CC_CP_FRONT_FINDER_HEADER