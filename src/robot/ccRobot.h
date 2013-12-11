#ifndef CC_ROBOT_HEADER
#define CC_ROBOT_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QPoint>
#include <QString>
#include <QMutex>


/*---------------------------------------------------------- Intern includes */
#include "../app/ccDisplayable.h"
#include "ccCommunicator.h"

namespace CCF
{
    class Robot: public Displayable, public QMutex
	{
        Q_OBJECT

		public:
            enum RobotMoves { 
                RM_FORWARD,                 // Forward movement
                RM_BACKWARD,                // Backward movement
                RM_STOP,                    // Stop sended
                RM_POSITION,                // Structure contains information concerning the counters
                RM_MANUAL_ROTATE_LEFT,      // Rotation on left using Manual speed command
                RM_MANUAL_ROTATE_RIGHT,     // Rotation on right using Manual speed command
                RM_AUTOMATIC_ROTATE_LEFT,   // Rotation on left using position to be reached command
                RM_AUTOMATIC_ROTATE_RIGHT,  // Rotation on right using position to be reached command
                RM_UNDEFINED                // Normally never used
            }; // enum RobotMoves

            struct CommandInformation
            {
                int timestamp;
                RobotMoves movement;
                int left;
                int right;
            }; // struct CommandInformation

            struct StatisticsInformation
            {
                int timestamp;
                int RSSI;
                int LQI;
                int Delay;
            }; // struct StatisticsInformation

            /// <summary>
            ///     Constructor
            ///</summary>
            Robot(const QString& _name, const QPoint& _position, Communicator* _communicator, const QString& _adress);

            /// <summary>
            ///     Destructor
            ///</summary>
            virtual ~Robot();

            virtual void ChangeOrientation(double rotation) = 0;
            virtual bool Connect();
            virtual bool Disconnect();
            const QList<CommandInformation>& GetCommandHistory() const;
            QList<StatisticsInformation> GetStatistics() const;
            const QString& GetAddress() const;
            QString GetInterfaceName() const;
            double GetOrientation() const;
            QPoint GetPosition() const;
            virtual QList<int> GetPositionCounters() = 0; // First value is the left wheel, second is the right wheel
            int GetUserRotationSpeed() const;
            int GetUserSpeed() const;
            inline void GoBackward()
                { GoBackward(userSpeed); }
            virtual void GoBackward(int speed)  = 0;
            inline void GoForward()
                { GoForward(userSpeed); }
            virtual void GoForward(int speed)   = 0;
            virtual bool IsConnected();
            inline void RotateLeft()
                { RotateLeft(userRotationSpeed); }
            virtual void RotateLeft(int speed)  = 0;
            void RotateRight()
                { RotateRight(userRotationSpeed); }
            virtual void RotateRight(int speed) = 0;
            void SetOrientation(double value);
            void SetPosition(const QPoint& newPosition);
            virtual void SetSpeed(int speedLeft, int speedRight) = 0;
            virtual void SetUserRotationSpeed(int _userRotationSpeed);
            virtual void SetUserSpeed(int _userSpeed);
            virtual void Stop() = 0;

        signals:
            void OrientationUpdated(int /*robot Identifier*/);
            void PositionUpdated(int /*robot Identifier*/);



        protected:
            /*--------------------------------------------- Protected methods*/            
            void addCommandToHistory(CommandInformation& info);
            void addStatistic(StatisticsInformation& stats);
            int generateTimeStamp();
            void sendCommand(QByteArray& command);
            virtual RobotMoves identifyMovement(int speedLeft, int speedRight) = 0;

            /*------------------------------------------ Protected attributes*/
            QString       address;
            QList<CommandInformation>    commandHistory;
            QList<StatisticsInformation> statistics;
            Communicator* communicator;
            QPoint        position;
            double        orientation;
            int           userSpeed;
            int           userRotationSpeed;

            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/


	}; // class Robot
} // namespace CCF

#endif // #ifndef CC_ROBOT_HEADER