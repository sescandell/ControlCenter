#ifndef CC_CALIBRATION_WINDOW_HEADER
#define CC_CALIBRATION_WINDOW_HEADER

/*----------------------------------------------------------- Extern includes*/
#include <QtGui/QWidget>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QProgressBar>


/*----------------------------------------------------------- Intern includes*/


namespace CCF
{
    

    class CalibrationWindow: public QWidget
    {
        Q_OBJECT

        public:
            struct ParamCalibration
                {
                    int boardWidth;
                    int boardHeight;
                    int boardCount;
                };
            CalibrationWindow(QWidget* displayer,QWidget* parent = 0, Qt::WindowFlags f = 0);
            ~CalibrationWindow();

			ParamCalibration GetBoardParameters();

        public slots:
            void OnBoardMatched();
            void OnCalibrationEnded();
            void OnRobotsSizeDefined();

        signals:            
            void on_btnQuit_clicked();
            void on_btnOk_clicked();
            void on_btnSaveUndistortion_clicked();
            void on_btnOpenUndistortion_clicked();
            void on_cbUndistortImage_stateChanged(int);
            void on_cbNormalizeImage_stateChanged(int);
            void startCalibration();
            void startDefineRobotsSize();
            void stopCalibration();
            void stopDefineRobotsSize();


        protected:
            enum MachineStates { MS_CALIBRATION_GUI_NORMAL_MODE,
                                 MS_CALIBRATION_GUI_UNDISTORTION_MODE,
                                 MS_CALIBRATION_GUI_DEFINE_SIZE_MODE
                                };

            void setWindowMachineState(MachineStates newMode);
            void updateInterface();

            MachineStates   windowMachineState;
            int             boardMatchedCount;
            bool            hasUndistortionParameters;
            bool            isRobotsSizeDefined;

        protected slots:
            void on_btnStartStopUndistortion_clicked();
            void on_btnDefineRobotsSize_clicked();
        
        /////////// Design
        protected:
            void setupInterface(QWidget* displayer);

            QGroupBox*      gbUndistortion;
            QGroupBox*      gbRobotsSize;
            QSpinBox*       sbBoardWidth;
            QSpinBox*       sbBoardHeight;
            QSpinBox*       sbBoardCount;
            QPushButton*    btnQuit;
            QPushButton*    btnOk;
            QPushButton*    btnStartStopUndistortion;
            QPushButton*    btnOpenUndistortion;
            QPushButton*    btnSaveUndistortion;
            QPushButton*    btnDefineRobotsSize;
            QCheckBox*      cbNormalizeImage;
            QCheckBox*      cbUndistortImage;
            QProgressBar*   pbUndistortion;
            QLabel*         laRobotSize;

        private:

    }; // class CalibrationWindow
} // namespace CCF

#endif // #ifndef CC_CALIBRATION_WINDOW_HEADER