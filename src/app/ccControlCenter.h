#ifndef CC_CONTROL_CENTER_HEADER
#define CC_CONTROL_CENTER_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QSize>

/*---------------------------------------------------------- Intern includes */
#include "../image/ccImageProcessing.h"
#include "../image/ccImageProcessor.h"
#include "../robot/ccRobot.h"
#include "../robot/ccCommunicator.h"
#include "../track/ccTrack.h"
#include "../controlUnit/ccControlUnit.h"

namespace CCF
{
    class ControlCenter: public QObject
	{
        Q_OBJECT

		public:

            static const int CC_CONTROL_CENTER_INFINITE_LAPS = -1;
            /// <summary>
            ///     Constructor
            ///</summary>
            ControlCenter();

            /// <summary>
            ///     Destructor
            ///</summary>
            ~ControlCenter();

            const Track* AddTrack(const QString& name, const Track* source);
            const Robot* AddKhepera(const QString& name, const QPoint& position, const QString& interfaceName, Communicator::Mode communicatorMode, const QString& address);            
            void AddRobotToFollowTrack(int robotId, int trackId, int lapsCount);
            void InitializeUndistortionCalibration(int boardWidth, int boardHeight, int boardCount);
            const IPKListPositions& GetFoundPositions() const;
			int GetFPS();
            QSize& GetVideoSize() const;
            const Robot* GetRobot(int robotId) const;
            QList<int> GetAssociatedRobotsToTrack(int trackId);
            void LoadCalibrationFromFile(QString& file);
            void MoveRobot(int identifier, int speedLeft, int speedRight);
            bool PositionExists(const QPoint& position) const;
            bool RemoveRobot(int identifier);
            bool RemoveTrack(int identifier);
            bool RemoveRobotTrackAssociation(int robotId);
            void SaveCalibrationToFile(QString& file);    
            void SetDisplayableImage(Image* image);
            void SetKheperaSize(int kSize);
            void StartCalibration();
            bool StartRobotFollowingTrack(int robotId);
            bool StopRobotFollowingTrack(int robotId);
            bool PauseRobotFollowingTrack(int robotId);
            void StartTrackingKheperaProcess();
            void StopProcessing();
            QString GetTrackName(int trackId) const;
            bool DefineTrackFollowingLapsTodo(int robotId, int value);
            void SetRobotRotationSpeed(int robotId, int value);
            void SetRobotMainSpeed(int robotId, int value);
            void SetCalibrationActivated(bool value);
            void SetNormalizationActivated(bool value);


        public slots:
            void OnNewPositionsFound(IPKListPositions positions);
            void OnSetCalibrationActivated(int value);
            void OnSetNormalizationActivated(int value);
            void SetDisplayActivated(int value);

        signals:
            void BoardMatched();
            void CalibrationEnded();
            void ImageProcessed();
            void EndOfTrackReached(int /*Robot identifier*/);
            void NewPositionsFindCount(int);
            

            
		protected:
            /*--------------------------------------------- Protected methods*/

            /*------------------------------------------ Protected attributes*/
            ControlUnit*        controlUnit;
            QMap<QString, Communicator* > robotCommunicators;
            ImageProcessing*    imageProcessing;
            IPKListPositions    lastPositions;
            QSemaphore          semSynchroImageControl;
            QMap<int, Robot*>   robots;
            int                 robotSize;
            QMap<int, Track*>   tracks;

        protected slots:
            void onEndOfTrackReached(int);

		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class ControlCenter
} // namespace CCF

#endif // #ifndef CC_CONTROL_CENTER_HEADER