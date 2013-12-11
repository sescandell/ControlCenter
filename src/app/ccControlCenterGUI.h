#ifndef CC_CONTROL_CENTER_GUI_HEADER
#define CC_CONTROL_CENTER_GUI_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QPushButton>

#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QPoint>
#include <QtCore/QTimer>

/*---------------------------------------------------------- Intern includes */
#include "ccControlCenter.h"
#include "../view/ccControlCenterView.h"
#include "../view/ccTrackView.h"
#include "ccControlCenterDisplayer.h"
#include "ccCalibrationWindow.h"
#include "ccRobotCreator.h"
#include "ccRobotController.h"
#include "ccTrackCreator.h"
#include "ccLayer.h"
#include "ccStatisticsDisplayer.h"


namespace CCF
{
    class ControlCenterGUI: public QMainWindow
	{
        Q_OBJECT

		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            ControlCenterGUI(QMainWindow *parent = 0);

            /// <summary>
            ///     Destructor
            ///</summary>
            virtual ~ControlCenterGUI();

        public slots:
            void OnNewPositionsFindCount(int count);
            void OnOrientationUpdated(int robotId);

        signals:
            void RobotsSizeDefined();


        protected:
            enum WindowModes {  CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT,
                                CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK, 
                                CC_CONTROL_CENTER_GUI_MODE_NORMAL,
                                CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL,
                                CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION,
                                CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE,
                                };
            /*--------------------------------------------- Protected methods*/
            void addTrack();
            void addTemporaryPointToTrack(const QPoint& point);
            void addPointToTrack(const QPoint& point);
            void addRobotInCircle(const QPoint& point);
            void addPointRobotsSize(const QPoint& position);
            void connectLayer(const Layer * layer);
            void disconnectLayer(const Layer * layer);
            void disableAddTrackMode();
            void removeRobot(int identifier);
            void removeTrack(int identifier);
            void setupInterface();
            void setWindowMode(WindowModes newMode);
            void updateInterface();
            void updatePositionsFindLabel(int count);
            void startCalibrationWindow();
            void startWindow();
            void stopCalibrationWindow();
            void setupFileMenu();
            void setupDisplayMenu();
            void setupHelpMenu();
            void initializeRobotOrientation(int robotId);


            /*------------------------------------------ Protected attributes*/
            ///////// Design objects
            // Main
            QWidget*                mainWidget;
            QVBoxLayout*            mainLayout;
            ControlCenterDisplayer* controlCenterDisplayer;
            ControlCenterDisplayer* histogramDisplayer;
            QGroupBox*              gbImageInformation;
            QGridLayout*            imgInformationLayout;
            QLabel*                 laWidth;
            QLabel*                 laHeight;
            QLabel*                 laFPS;
            QLabel*                 laPositionsFind;
            QCheckBox*              cbCalibrateImage;
            QCheckBox*              cbNormalizeImage;
            QCheckBox*              cbDisplayImage;
            // Robots
            QDockWidget*            robotsDock;
            QWidget*                robotsDockWidget;
            QVBoxLayout*            robotsDockLayout;
            QHBoxLayout*            robotsActions;
            QVBoxLayout*            robotsLayers;
            RobotCreator*           robotCreator;
            RobotController*        robotsController;
            QPushButton*            btnAddRobot;
            // Tracks
            QDockWidget*            tracksDock;
            QWidget*                tracksDockWidget;
            QVBoxLayout*            tracksDockLayout;
            QHBoxLayout*            tracksActions;
            QVBoxLayout*            tracksLayers;       // Useless ... but to keep the same
                                                        // organization as the robots
            TrackCreator*           trackCreator;
            QPushButton*            btnAddTrack;
            // Statistics
            QDockWidget*            statsDock;
            QWidget*                statsDockWidget;
            QVBoxLayout*            statsDockLayout;
            QHBoxLayout*            statsActions;
            QVBoxLayout*            statsDisplayers;    // Useless ... but to keep the same
                                                        // organization as the robots
            QCheckBox*              cbDisplayRSSI;
            QCheckBox*              cbDisplayLQI;
            QCheckBox*              cbDisplayDelay;
            // Layers
            QMap<int /* object identifier*/, Layer*> layers;

            // Statistics container
            QMap<int /* robot identifier*/, StatisticsDisplayer*> statisticsDisplayers;
            
            /////// Core Objects
            CalibrationWindow*      calibrationWindow;
            ControlCenter*          controlCenter;            
            ControlCenterView*      controlCenterView;
            Track*                  tempTrack;
            TrackView*              tempTrackView;
            WindowModes             windowMode;
            QPoint                  robotsSizePoint1;
            QPoint                  robotsSizePoint2;
            QPoint                  robotsSizeLastPointClicked;
            QTimer                  timerFPS;

        protected slots:
            void onCalibrationCanceled();
            void onCalibrationOk();
            void onCalibrationEnded();
            void deleteObject(int layerType, int objectIdentifier);
            void layerClicked(int layerType, int objectIdentifier);
            void onLoadCalibration();
            void on_btnAddRobot_clicked();
            void on_btnAddTrack_clicked();
            void on_cbDisplayRSSI_stateChanged(int);
            void on_cbDisplayLQI_stateChanged(int);
            void on_cbDisplayDelay_stateChanged(int);
            void on_controlCenterDisplayer_clicked(QPoint);
            void on_controlCenterDisplayer_doubleClicked(QPoint);
            void on_controlCenterDisplayer_mouseMoved(QPoint);
            void on_robotsController_Backward();
            void on_robotsController_Forward();            
            void on_robotsController_RotateLeft();
            void on_robotsController_RotateRight();
            void on_robotsController_Stop();
            void onSaveCalibration();
            void onStartCalibration();
            void onStartDefineRobotsSize();
            void onStopCalibration();
            void onStopDefineRobotsSize();
            void onAddTrackRobotAssociationClicked(int robotId);
            void onRemoveTrackRobotAssociationClicked(int robotId);
            void onStartTrackFollowerClicked(int robotId);
            void onStopTrackFollowerClicked(int robotId);
            void onPauseTrackFollowerClicked(int robotId);
            void onRepeatTrackFollowerClicked(int robotId, bool activated);
            void onEndOfTrackReached(int robotId);
            void onRotationSpeedChanged(int robotId, int value);
            void onMainSpeedChanged(int robotId, int value);
            void onDisplayActionClicked();
            void onCalibrateActionClicked();
            void onNormalizeActionClicked();
            void onHelpActionClicked();
            void onTimerFPSTimeOut();
            void onShowHideRobotPanelActionClicked();
            void onShowHideTrackPanelActionClicked();
            void onShowHideHistogramActionClicked();
            

            
            
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class ControlCenterGUI
} // namespace CCF

#endif // #ifndef CC_CONTROL_CENTER_GUI_HEADER