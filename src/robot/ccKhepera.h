#ifndef CC_KHEPERA_HEADER
#define CC_KHEPERA_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QString>
#include <QByteArray>
#include <QList>

/*---------------------------------------------------------- Intern includes */
#include "ccRobot.h"

namespace CCF
{
    class Khepera: public Robot
    {
        public: 
            static const int CC_KHEPERA_MAX_SPEED_LEFT     = 127;
            static const int CC_KHEPERA_MAX_SPEED_RIGHT    = 127;
            static const int CC_KHEPERA_STOP_SPEED_LEFT    = 0;
            static const int CC_KHEPERA_STOP_SPEED_RIGHT   = 0;
            static const double CC_KH_ROTATION_COEFFICIENT;            

            Khepera(const QString& _name, const QPoint& _position, Communicator* _communicator, const QString& _address);
            ~Khepera();

            void ChangeOrientation(double rotation);
            QList<int> GetPositionCounters(); // First value is the left wheel, second is the right wheel
            void GoForward(int speed);
            void GoBackward(int speed);
            void ResetPositionCounters();
            void Reset();
            void RotateLeft(int speed);
            void RotateRight(int speed);
            void SetConfigurationSpeedProfile(int maxSpeedLeft, int accLeft, int maxSpeedRight, int accRight);
            void SetPositionToBeReached(int posLeft, int posRight);
            void SetSpeed(int speedLeft, int speedRight);
            void Stop();
            

        protected:            
            void addCurrentPositionToHistory();
            RobotMoves identifyMovement(int speedLeft, int speedRight);
            void processStatisticsData(QList<QByteArray>& statisticsData);

            int automaticSpeedLeft;
            int automaticSpeedRight;

        private:

    }; // class Khepera
} // namespace CCF

#endif