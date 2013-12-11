#include "ccCalibrationWindow.h"

#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QSizePolicy>
#include <QtGui/QSpacerItem>

#include <QtCore/QMetaObject>
#include <QtCore/QString>



namespace CCF
{

    CalibrationWindow::CalibrationWindow(QWidget* displayer,QWidget* parent, Qt::WindowFlags f)
        :QWidget(parent,f), isRobotsSizeDefined(false), hasUndistortionParameters(false)
    {
        setupInterface(displayer);
        setWindowState(Qt::WindowMaximized);
        setWindowMachineState(MS_CALIBRATION_GUI_NORMAL_MODE);
        
        // Connection
        connect(btnOk,   SIGNAL(clicked()), this, SLOT(hide()));
        connect(btnQuit, SIGNAL(clicked()), this, SLOT(hide()));
    }

    CalibrationWindow::~CalibrationWindow()
    {
        // TODO
    }

    void CalibrationWindow::setWindowMachineState(CalibrationWindow::MachineStates newMode)
    {
        windowMachineState = newMode;
        updateInterface();
    }

    void CalibrationWindow::updateInterface()
    {
        switch(windowMachineState)
        {
            case MS_CALIBRATION_GUI_NORMAL_MODE:
                gbUndistortion->setEnabled(true);
                gbRobotsSize->setEnabled(true);
                btnOk->setEnabled(isRobotsSizeDefined);
                btnQuit->setEnabled(true);
                btnSaveUndistortion->setEnabled(hasUndistortionParameters);
                btnStartStopUndistortion->setEnabled(true);
                btnStartStopUndistortion->setStyleSheet(QString::fromUtf8("QPushButton#btnStartStopUndistortion\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/start.png);\n"
                      "    margin: 0;\n"
                      "    padding: 0;\n"
                      "    border: none;\n"
                      "}\n"
                      "\n"
                      "QPushButton#btnStartStopUndistortion:hover\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/start_light.png);\n"
                      "}"));
                btnStartStopUndistortion->setToolTip(QString::fromUtf8("Start undistortion"));
                btnOpenUndistortion->setEnabled(true);
                break;
            case MS_CALIBRATION_GUI_UNDISTORTION_MODE:
                gbUndistortion->setEnabled(false);
                gbRobotsSize->setEnabled(false);
                btnOk->setEnabled(false);
                btnQuit->setEnabled(false);
                btnSaveUndistortion->setEnabled(false);
                btnOpenUndistortion->setEnabled(false);
                btnStartStopUndistortion->setEnabled(true);
                btnStartStopUndistortion->setStyleSheet(QString::fromUtf8("QPushButton#btnStartStopUndistortion\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/stop.png);\n"
                      "    margin: 0;\n"
                      "    padding: 0;\n"
                      "    border: none;\n"
                      "}\n"
                      "\n"
                      "QPushButton#btnStartStopUndistortion:hover\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/stop_light.png);\n"
                      "}"));
                btnStartStopUndistortion->setToolTip(QString::fromUtf8("Stop undistortion"));
                break;
            case MS_CALIBRATION_GUI_DEFINE_SIZE_MODE:
                gbUndistortion->setEnabled(false);
                gbRobotsSize->setEnabled(true);
                btnOk->setEnabled(false);
                btnQuit->setEnabled(false);
                btnSaveUndistortion->setEnabled(false);
                btnOpenUndistortion->setEnabled(false);
                btnStartStopUndistortion->setEnabled(false);
                break;
            default:
                // Normally never happened
                break;
        };
    }

    void CalibrationWindow::setupInterface(QWidget* displayer)
    {
        QGridLayout* mainLayout = new QGridLayout(this);
            QVBoxLayout* verticalLayout = new QVBoxLayout();
                gbUndistortion = new QGroupBox(this);
                gbUndistortion->setMaximumWidth(250);
                gbUndistortion->setTitle(QString::fromUtf8("Undistortion configuration"));
                    QFormLayout* undistortionLayout = new QFormLayout(gbUndistortion);
                        sbBoardHeight = new QSpinBox(gbUndistortion);
                        sbBoardHeight->setMinimum(3);
                    undistortionLayout->addRow(new QLabel(QString::fromUtf8("Board height"),gbUndistortion),sbBoardHeight);
                        sbBoardWidth = new QSpinBox(gbUndistortion);
                        sbBoardWidth->setMinimum(3);
                    undistortionLayout->addRow(new QLabel(QString::fromUtf8("Board width"),gbUndistortion),sbBoardWidth);
                        sbBoardCount = new QSpinBox(gbUndistortion);
                        sbBoardCount->setMinimum(3);
                    undistortionLayout->addRow(new QLabel(QString::fromUtf8("Board count"),gbUndistortion),sbBoardCount);
                        cbUndistortImage = new QCheckBox(QString::fromUtf8("Undistort image"),gbUndistortion);
                        cbUndistortImage->setChecked(true);
                        cbUndistortImage->setObjectName(QString::fromUtf8("cbUndistortImage"));
                    undistortionLayout->addRow(cbUndistortImage);
            verticalLayout->addWidget(gbUndistortion);
                QHBoxLayout* tempLayout = new QHBoxLayout();
                    pbUndistortion = new QProgressBar(this);
                    pbUndistortion->setMaximumWidth(180);
                    pbUndistortion->setMinimum(0);
                    pbUndistortion->setMaximum(100);
                    pbUndistortion->setTextVisible(true);
                tempLayout->addWidget(pbUndistortion);
                    btnStartStopUndistortion = new QPushButton(QString(),this);
                    btnStartStopUndistortion->setObjectName("btnStartStopUndistortion");
                    btnStartStopUndistortion->setFixedSize(16,16);
                    btnStartStopUndistortion->setStyleSheet(QString::fromUtf8("QPushButton#btnStartStopUndistortion\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/start.png);\n"
                      "    margin: 0;\n"
                      "    padding: 0;\n"
                      "    border: none;\n"
                      "}\n"
                      "\n"
                      "QPushButton#btnStartStopUndistortion:hover\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/start_light.png);\n"
                      "}"));
                    btnStartStopUndistortion->setToolTip(QString::fromUtf8("Start undistortion"));
                tempLayout->addWidget(btnStartStopUndistortion);
                    btnOpenUndistortion = new QPushButton(QString(),this);
                    btnOpenUndistortion->setObjectName("btnOpenUndistortion");
                    btnOpenUndistortion->setFixedSize(16,16);
                    btnOpenUndistortion->setStyleSheet(QString::fromUtf8("QPushButton#btnOpenUndistortion\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/open.png);\n"
                      "    margin: 0;\n"
                      "    padding: 0;\n"
                      "    border: none;\n"
                      "}\n"
                      "\n"
                      "QPushButton#btnOpenUndistortion:hover\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/open_light.png);\n"
                      "}"));
                    btnOpenUndistortion->setToolTip(QString::fromUtf8("Open file ..."));
                tempLayout->addWidget(btnOpenUndistortion);
                    btnSaveUndistortion = new QPushButton(QString(),this);
                    btnSaveUndistortion->setObjectName("btnSaveUndistortion");
                    btnSaveUndistortion->setFixedSize(16,16);
                    btnSaveUndistortion->setStyleSheet(QString::fromUtf8("QPushButton#btnSaveUndistortion\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/save.png);\n"
                      "    margin: 0;\n"
                      "    padding: 0;\n"
                      "    border: none;\n"
                      "}\n"
                      "\n"
                      "QPushButton#btnSaveUndistortion:hover\n"
                      "{	\n"
                      "	background-image: url(:/icons/ressources/save_light.png);\n"
                      "}"));
                    btnSaveUndistortion->setToolTip(QString::fromUtf8("Save to file ..."));
                tempLayout->addWidget(btnSaveUndistortion);
            verticalLayout->addLayout(tempLayout);
                gbRobotsSize = new QGroupBox(this);
                gbRobotsSize->setMaximumWidth(250);
                gbRobotsSize->setTitle(QString::fromUtf8("Robots"));
                    QGridLayout* tempRobotsLayout = new QGridLayout(gbRobotsSize);
                    tempRobotsLayout->addWidget(new QLabel(QString::fromUtf8("Size :"),gbRobotsSize),0,0);
                        laRobotSize = new QLabel(QString::fromUtf8("undefined"),gbRobotsSize);
                    tempRobotsLayout->addWidget(laRobotSize,0,1);
                        btnDefineRobotsSize = new QPushButton(QString(),gbRobotsSize);
                        btnDefineRobotsSize->setObjectName(QString::fromUtf8("btnDefineRobotsSize"));
                        btnDefineRobotsSize->setFixedSize(16,16);
                        btnDefineRobotsSize->setStyleSheet(QString::fromUtf8("QPushButton#btnDefineRobotsSize\n"
                          "{	\n"
                          "	background-image: url(:/icons/ressources/size.png);\n"
                          "    margin: 0;\n"
                          "    padding: 0;\n"
                          "    border: none;\n"
                          "}\n"
                          "\n"
                          "QPushButton#btnDefineRobotsSize:hover\n"
                          "{	\n"
                          "	background-image: url(:/icons/ressources/size_light.png);\n"
                          "}"));
                        btnDefineRobotsSize->setToolTip(QString::fromUtf8("Define robots size"));
                    tempRobotsLayout->addWidget(btnDefineRobotsSize,0,2);
            verticalLayout->addWidget(gbRobotsSize);
                cbNormalizeImage = new QCheckBox(QString::fromUtf8("Normalize image histogram"),this);
                cbNormalizeImage->setChecked(false);
                cbNormalizeImage->setObjectName(QString::fromUtf8("cbNormalizeImage"));
            verticalLayout->addWidget(cbNormalizeImage);
            verticalLayout->addItem(new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Expanding));
                tempLayout = new QHBoxLayout();
                    btnQuit = new QPushButton(QString::fromUtf8("Quit"),this);
                    btnQuit->setObjectName(QString::fromUtf8("btnQuit"));
                tempLayout->addWidget(btnQuit);
                    btnOk = new QPushButton(QString::fromUtf8("Ok"),this);
                    btnOk->setObjectName(QString::fromUtf8("btnOk"));
                tempLayout->addWidget(btnOk);
            verticalLayout->addLayout(tempLayout);
        mainLayout->addLayout(verticalLayout,0,0);
        mainLayout->addWidget(displayer,0,1);

        QMetaObject::connectSlotsByName(this);
    }

    void CalibrationWindow::on_btnStartStopUndistortion_clicked()
    {
        if(MS_CALIBRATION_GUI_NORMAL_MODE==windowMachineState) // Start Calibration
        {
            pbUndistortion->setValue(0);
		    boardMatchedCount = 0;
            setWindowMachineState(MS_CALIBRATION_GUI_UNDISTORTION_MODE);
            emit startCalibration();
        }
        else    // Stop calibration
        {
            emit stopCalibration();
            setWindowMachineState(MS_CALIBRATION_GUI_NORMAL_MODE);
        }   
    }

    void CalibrationWindow::on_btnDefineRobotsSize_clicked()
    {
        if(MS_CALIBRATION_GUI_DEFINE_SIZE_MODE==windowMachineState)
        {
            setWindowMachineState(MS_CALIBRATION_GUI_NORMAL_MODE);
            emit stopDefineRobotsSize();
        }
        else
        {
            setWindowMachineState(MS_CALIBRATION_GUI_DEFINE_SIZE_MODE);
            emit startDefineRobotsSize();
        }
    }

    void CalibrationWindow::OnBoardMatched()
    {
        pbUndistortion->setValue(++boardMatchedCount*100/sbBoardCount->value());
    }

    void CalibrationWindow::OnCalibrationEnded()
    {
        hasUndistortionParameters = true;
        setWindowMachineState(MS_CALIBRATION_GUI_NORMAL_MODE);
    }

    void CalibrationWindow::OnRobotsSizeDefined()
    {
        isRobotsSizeDefined = true;
        laRobotSize->setText(QString::fromUtf8("Done !"));
        setWindowMachineState(MS_CALIBRATION_GUI_NORMAL_MODE);
    }

	CalibrationWindow::ParamCalibration CalibrationWindow::GetBoardParameters()
	{
		ParamCalibration params;
		
		params.boardWidth  = sbBoardWidth->value();
		params.boardHeight = sbBoardHeight->value();
		params.boardCount  = sbBoardCount->value();

		return params;
	}
} // namesapce CCF