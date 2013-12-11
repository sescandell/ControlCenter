#include "ccControlCenterGUI.h"

#include <QtCore/QMetaObject>
#include <QtCore/QString>
#include <QtCore/QSize>

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QScrollArea>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QMenu>
#include <QtGui/QKeySequence>
#include <QtGui/QIcon>

#include "../view/ccView.h"
#include "../view/ccRobotView.h"
#include "../robot/ccRobot.h"
#include "../robot/ccCommunicator.h"
#include "ccRobotLayer.h"
#include "ccTrackLayer.h"


namespace CCF
{
    static const int CC_CONTROL_CENTER_GUI_DEFAULT_LAPS_COUNT     = 1;
    static const int CC_CONTROL_CENTER_GUI_SPEED_STOP             = 0;
    static const int CC_CONTROL_CENTER_GUI_SPEED_INIT_ORIENTATION = 10;
    static const int CC_CONTROL_CENTER_GUI_TIMER_FPS              = 1000;


    ControlCenterGUI::ControlCenterGUI(QMainWindow *parent)
        :QMainWindow(parent)
    {
        // TODO : SplashScreen

        setupInterface();
        
        setWindowState(Qt::WindowMaximized);

        calibrationWindow       = 0;
        tempTrack               = 0;
        tempTrackView           = 0;
        windowMode              = CC_CONTROL_CENTER_GUI_MODE_NORMAL;
        controlCenter           = new ControlCenter();
        controlCenterView       = new ControlCenterView(controlCenter,controlCenterDisplayer->size(), histogramDisplayer->size());

        View::DefineControlCenterView(controlCenterView);
        controlCenter->SetDisplayableImage(controlCenterView->GetImage());
        controlCenterView->SetDisplayCirclesActivated(false);

        connect(controlCenter,          SIGNAL(EndOfTrackReached(int)),     this,                   SLOT(onEndOfTrackReached(int)));
        connect(controlCenter,          SIGNAL(NewPositionsFindCount(int)), this,                   SLOT(OnNewPositionsFindCount(int)));
        connect(controlCenter,          SIGNAL(ImageProcessed()),           controlCenterDisplayer, SLOT(update()));
        //connect(controlCenter,          SIGNAL(ImageProcessed()),           histogramDisplayer,     SLOT(update()));
        connect(controlCenterDisplayer, SIGNAL(resized(QSize)),             controlCenterView,      SLOT(DisplayerResized(QSize)));
        //connect(histogramDisplayer,     SIGNAL(resized(QSize)),             controlCenterView,      SLOT(HistogramDisplayerResized(QSize)));
        connect(controlCenterDisplayer, SIGNAL(painting(QPainter&)),        controlCenterView,      SLOT(Draw(QPainter&)),Qt::DirectConnection);
        //connect(histogramDisplayer,     SIGNAL(painting(QPainter&)),        controlCenterView,      SLOT(DrawHistogram(QPainter&)),Qt::DirectConnection);
        connect(cbDisplayImage,         SIGNAL(stateChanged(int)),          controlCenterView,      SLOT(SetDisplayImageActivated(int)));
        connect(cbDisplayImage,         SIGNAL(stateChanged(int)),          controlCenter,          SLOT(SetDisplayActivated(int)));
        connect(cbCalibrateImage,       SIGNAL(stateChanged(int)),          controlCenter,          SLOT(OnSetCalibrationActivated(int)));
        connect(cbNormalizeImage,       SIGNAL(stateChanged(int)),          controlCenter,          SLOT(OnSetNormalizationActivated(int)));

        startCalibrationWindow();
    }

    ControlCenterGUI::~ControlCenterGUI()
    {        
        timerFPS.stop();
        disconnect(controlCenter, SIGNAL(ImageProcessed()), histogramDisplayer, SLOT(update()));
        disconnect(controlCenter, SIGNAL(ImageProcessed()), controlCenterDisplayer, SLOT(update()));
        controlCenter->SetDisplayableImage(0);
        
        delete tempTrack;
        delete trackCreator;
        delete robotCreator;
        delete histogramDisplayer;
        delete controlCenterDisplayer;
        delete controlCenterView;
        delete controlCenter;
    }
    
    void ControlCenterGUI::setupInterface()
    {
      QLabel*       laTemp;
      if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("MainWindow"));
      QApplication::setWindowIcon(QIcon(QString::fromUtf8(":/icons/ressources/controller.png")));
      setupFileMenu();
      setupDisplayMenu();
      setupHelpMenu();
      ////////// Main widget
        mainWidget = new QWidget(this);
        mainWidget->setObjectName(QString::fromUtf8("mainWidget"));
        mainLayout = new QVBoxLayout(mainWidget);
        //// Displayer
          controlCenterDisplayer = new ControlCenterDisplayer();
          controlCenterDisplayer->setObjectName(QString::fromUtf8("controlCenterDisplayer"));
        //// Groupbox
          gbImageInformation   = new QGroupBox(mainWidget);
          gbImageInformation->setMaximumHeight(100);
          gbImageInformation->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
          imgInformationLayout = new QGridLayout(gbImageInformation);
        // Width
          laTemp               = new QLabel(gbImageInformation);
          laTemp->setText("Width :");
          imgInformationLayout->addWidget(laTemp,0,0,1,1);
          laWidth              = new QLabel(gbImageInformation);
          laWidth->setText("0000 px");
          imgInformationLayout->addWidget(laWidth,0,1,1,1);
        // Height
          laTemp               = new QLabel(gbImageInformation);
          laTemp->setText("Height :");
          imgInformationLayout->addWidget(laTemp,1,0,1,1);
          laHeight             = new QLabel(gbImageInformation);
          laHeight->setText("0000 px");
          imgInformationLayout->addWidget(laHeight,1,1,1,1);
        // FPS
          laTemp               = new QLabel(gbImageInformation);
          laTemp->setText("FPS :");
          imgInformationLayout->addWidget(laTemp,2,0,1,1);
          laFPS                = new QLabel(gbImageInformation);
          laFPS->setText("0 FPS");
          imgInformationLayout->addWidget(laFPS,2,1,1,1);
        // Positions find
          laTemp               = new QLabel(gbImageInformation);
          laTemp->setText("Positions find :");
          imgInformationLayout->addWidget(laTemp,3,0,1,1);
          laPositionsFind      = new QLabel(gbImageInformation);
          laPositionsFind->setText("0");
          imgInformationLayout->addWidget(laPositionsFind,3,1,1,1);
        // Calibrate image
          cbCalibrateImage     = new QCheckBox(gbImageInformation);
          cbCalibrateImage->setObjectName("cbCalibrateImage");
          cbCalibrateImage->setChecked(false);
          cbCalibrateImage->setText("Calibrate Image");
          imgInformationLayout->addWidget(cbCalibrateImage,0,2,1,1);
        // Normalize image
          cbNormalizeImage     = new QCheckBox(gbImageInformation);
          cbNormalizeImage->setObjectName("cbNormalizeImage");
          cbNormalizeImage->setChecked(false);
          cbNormalizeImage->setText("Normalize image histogram");
          imgInformationLayout->addWidget(cbNormalizeImage,1,2,1,1);
        // Display image
          cbDisplayImage       = new QCheckBox(gbImageInformation);
          cbDisplayImage->setObjectName("cbDisplayImage");
          cbDisplayImage->setChecked(true);
          cbDisplayImage->setText("Display Image");
          imgInformationLayout->addWidget(cbDisplayImage,2,2,1,1);
        // Histogram Displayer
          histogramDisplayer = new ControlCenterDisplayer();
          histogramDisplayer->setObjectName(QString::fromUtf8("histogramDisplayer"));
          histogramDisplayer->setMaximumSize(200,100);
          histogramDisplayer->setVisible(false);
      //// Adding objects to the layout
        QHBoxLayout* horizLayout = new QHBoxLayout();
            horizLayout->addWidget(gbImageInformation);
            horizLayout->addWidget(histogramDisplayer);
            horizLayout->setAlignment(Qt::AlignLeft);
        mainLayout->addWidget(controlCenterDisplayer);
        mainLayout->addLayout(horizLayout);
        setCentralWidget(mainWidget);
      ////////// Robots
        // Dock
        robotsDock = new QDockWidget(QString::fromUtf8("Robots"),this);
          robotsDock->setObjectName("robotsDock");
          robotsDockWidget = new QWidget();
          robotsDockWidget->setObjectName("robotsDockWidget");
          robotsDockWidget->setMinimumSize(230,50);
          robotsDockLayout = new QVBoxLayout(robotsDockWidget);
        // Header
          robotsActions    = new QHBoxLayout();
          robotsActions->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
          btnAddRobot      = new QPushButton(robotsDockWidget);
          btnAddRobot->setObjectName("btnAddRobot");
          btnAddRobot->setFixedSize(16,16);
          btnAddRobot->setStyleSheet(QString::fromUtf8("QPushButton#btnAddRobot\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/robot_add.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnAddRobot:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/robot_add_light.png);\n"
            "}"));
          btnAddRobot->setToolTip(QString::fromUtf8("Add a robot"));
          robotsActions->addWidget(btnAddRobot);
          robotsDockLayout->addLayout(robotsActions);
        // Creator
          robotCreator = new RobotCreator();
          robotCreator->setObjectName("robotCreator");
          robotCreator->setVisible(false);
          robotsDockLayout->addWidget(robotCreator);
        // Layers          
          QScrollArea* robotsScrollArea = new QScrollArea();
          robotsScrollArea->setObjectName(QString::fromUtf8("robotsScrollArea"));
          robotsScrollArea->setMinimumWidth(250);
          robotsScrollArea->setStyleSheet(QString::fromUtf8("QScrollArea#robotsScrollArea\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}"));
          robotsScrollArea->setWidgetResizable(true);
          QWidget* robotsScrollAreaWidget = new QWidget();
          robotsScrollAreaWidget->setObjectName(QString::fromUtf8("robotsScrollAreaWidget"));
          robotsScrollAreaWidget->setStyleSheet(QString::fromUtf8("QWidget#robotsScrollAreaWidget\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "}"));
          robotsLayers = new QVBoxLayout(robotsScrollAreaWidget);
          robotsLayers->setAlignment(Qt::AlignTop);
          robotsScrollArea->setWidget(robotsScrollAreaWidget);
          robotsDockLayout->addWidget(robotsScrollArea);
        // Controller
          robotsController = new RobotController();
          robotsController->setObjectName("robotsController");
          robotsController->setVisible(true);
          robotsDockLayout->addWidget(robotsController);
        robotsDock->setWidget(robotsDockWidget);
        addDockWidget(Qt::RightDockWidgetArea, robotsDock);
      ////////// Tracks dock
        // Dock
          tracksDock = new QDockWidget(QString::fromUtf8("Tracks"),this);
          tracksDock->setObjectName("tracksDock");
          tracksDockWidget = new QWidget();
          tracksDockWidget->setMinimumSize(170,50);
          tracksDockWidget->setObjectName("tracksDockWidget");
          tracksDockLayout = new QVBoxLayout(tracksDockWidget);
        // Header
          tracksActions    = new QHBoxLayout();
          tracksActions->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
          btnAddTrack      = new QPushButton(tracksDockWidget);
          btnAddTrack->setObjectName("btnAddTrack");
          btnAddTrack->setFixedSize(16,16);
          btnAddTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnAddTrack\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/track_add.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnAddTrack:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/track_add_light.png);\n"
            "}"));
          btnAddTrack->setToolTip(QString::fromUtf8("Add a track"));
          tracksActions->addWidget(btnAddTrack);
          tracksDockLayout->addLayout(tracksActions);
        // Creator
          trackCreator = new TrackCreator();
          trackCreator->setObjectName("trackCreator");
          trackCreator->setVisible(false);
          tracksDockLayout->addWidget(trackCreator);
        // Layers
          QScrollArea* tracksScrollArea = new QScrollArea();
          tracksScrollArea->setObjectName(QString::fromUtf8("tracksScrollArea"));
          tracksScrollArea->setMinimumWidth(150);
          tracksScrollArea->setStyleSheet(QString::fromUtf8("QScrollArea#tracksScrollArea\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}"));
          tracksScrollArea->setWidgetResizable(true);
          QWidget* tracksScrollAreaWidget = new QWidget();
          tracksScrollAreaWidget->setObjectName(QString::fromUtf8("tracksScrollAreaWidget"));
          tracksScrollAreaWidget->setStyleSheet(QString::fromUtf8("QWidget#tracksScrollAreaWidget\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "}"));
          tracksLayers = new QVBoxLayout(tracksScrollAreaWidget);
          tracksLayers->setAlignment(Qt::AlignTop);
          tracksScrollArea->setWidget(tracksScrollAreaWidget);
          tracksDockLayout->addWidget(tracksScrollArea);
        tracksDock->setWidget(tracksDockWidget);
        addDockWidget(Qt::LeftDockWidgetArea, tracksDock);
        ////////// Statistics displayer dock
        // Dock
          statsDock = new QDockWidget(QString::fromUtf8("Communication quality"),this);
          statsDock->setObjectName("statsDock");
          statsDockWidget = new QWidget();
          statsDockWidget->setMinimumSize(170,50);
          statsDockWidget->setObjectName("statsDockWidget");
          statsDockLayout = new QVBoxLayout(statsDockWidget);
        // Header
          statsActions    = new QHBoxLayout();
          cbDisplayRSSI = new QCheckBox(QString::fromUtf8("RSSI"));
          cbDisplayRSSI->setChecked(false);
          cbDisplayRSSI->setObjectName("cbDisplayRSSI");
          statsActions->addWidget(cbDisplayRSSI);
          cbDisplayLQI = new QCheckBox(QString::fromUtf8("LQI"));
          cbDisplayLQI->setChecked(false);
          cbDisplayLQI->setObjectName("cbDisplayLQI");
          statsActions->addWidget(cbDisplayLQI);
          cbDisplayDelay = new QCheckBox(QString::fromUtf8("Delay"));
          cbDisplayDelay->setChecked(true);
          cbDisplayDelay->setObjectName("cbDisplayDelay");
          statsActions->addWidget(cbDisplayDelay);
          statsDockLayout->addLayout(statsActions);
        // Displayers
          QScrollArea* statsScrollArea = new QScrollArea();
          statsScrollArea->setObjectName(QString::fromUtf8("statsScrollArea"));
          statsScrollArea->setMinimumWidth(150);
          statsScrollArea->setStyleSheet(QString::fromUtf8("QScrollArea#statsScrollArea\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}"));
          statsScrollArea->setWidgetResizable(true);
          QWidget* statsScrollAreaWidget = new QWidget();
          statsScrollAreaWidget->setObjectName(QString::fromUtf8("statsScrollAreaWidget"));
          statsScrollAreaWidget->setStyleSheet(QString::fromUtf8("QWidget#statsScrollAreaWidget\n"
            "{	\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "}"));
          statsDisplayers = new QVBoxLayout(statsScrollAreaWidget);
          statsDisplayers->setAlignment(Qt::AlignTop);
          statsScrollArea->setWidget(statsScrollAreaWidget);
          statsDockLayout->addWidget(statsScrollArea);
        statsDock->setWidget(statsDockWidget);
        addDockWidget(Qt::RightDockWidgetArea, statsDock);

        QMetaObject::connectSlotsByName(this);
    } // --- end of setupInterface()

    void ControlCenterGUI::startCalibrationWindow()
    {
        calibrationWindow = new CalibrationWindow(controlCenterDisplayer);
        
        //////////// Connect signals
        connect(calibrationWindow,  SIGNAL(on_btnQuit_clicked()),                  this,                SLOT(onCalibrationCanceled()));
        connect(calibrationWindow,  SIGNAL(on_btnOk_clicked()),                    this,                SLOT(onCalibrationOk()));
        connect(calibrationWindow,  SIGNAL(on_btnSaveUndistortion_clicked()),      this,                SLOT(onSaveCalibration()));
        connect(calibrationWindow,  SIGNAL(on_btnOpenUndistortion_clicked()),      this,                SLOT(onLoadCalibration()));
        connect(calibrationWindow,  SIGNAL(on_cbUndistortImage_stateChanged(int)), controlCenter,       SLOT(OnSetCalibrationActivated(int)));
        connect(calibrationWindow,  SIGNAL(on_cbNormalizeImage_stateChanged(int)), controlCenter,       SLOT(OnSetNormalizationActivated(int)));
        connect(calibrationWindow,  SIGNAL(startCalibration()),                    this,                SLOT(onStartCalibration()));
        connect(calibrationWindow,  SIGNAL(stopCalibration()),                     this,                SLOT(onStopCalibration()));
        connect(calibrationWindow,  SIGNAL(startDefineRobotsSize()),               this,                SLOT(onStartDefineRobotsSize()));
        connect(calibrationWindow,  SIGNAL(stopDefineRobotsSize()),                this,                SLOT(onStopDefineRobotsSize()));
        connect(controlCenter,      SIGNAL(BoardMatched()),                        calibrationWindow,   SLOT(OnBoardMatched()));
        connect(controlCenter,      SIGNAL(CalibrationEnded()),                    calibrationWindow,   SLOT(OnCalibrationEnded()));
        connect(controlCenter,      SIGNAL(CalibrationEnded()),                    this,                SLOT(onCalibrationEnded()));
        connect(this,               SIGNAL(RobotsSizeDefined()),                   calibrationWindow,   SLOT(OnRobotsSizeDefined()));

        /////// Show the window
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL);
        calibrationWindow->show();
    }

    void ControlCenterGUI::setWindowMode(ControlCenterGUI::WindowModes newMode)
    {
        windowMode = newMode;
        updateInterface();
    }

    void ControlCenterGUI::updateInterface()
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK:
                trackCreator->setVisible(true);
                robotsDock->setEnabled(false);
                statusBar()->showMessage(QString::fromUtf8("Fill parameters fields and draw your track."));
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT:
                robotCreator->setVisible(true);
                tracksDock->setEnabled(false);
                controlCenterView->SetDisplayCirclesActivated(true);
                statusBar()->showMessage(QString::fromUtf8("Fill parameters fields and select a circle on image."));
                break;
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
                robotCreator->setVisible(false);
                robotsDock->setEnabled(true);
                trackCreator->setVisible(false);
                tracksDock->setEnabled(true);
                controlCenterView->SetDisplayCirclesActivated(false);
                statusBar()->clearMessage();
                break;
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE:
            default:
                break;
        }; // switch
    }

    void ControlCenterGUI::onCalibrationCanceled()
    {
        stopCalibrationWindow();
        close();
    }

    void ControlCenterGUI::onCalibrationOk()
    {
        // Close properly the calibration Window
        stopCalibrationWindow();
        // Start main window
        startWindow();
    }

    void ControlCenterGUI::onCalibrationEnded()
    {
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL);
    }
    void ControlCenterGUI::on_controlCenterDisplayer_clicked(QPoint position)
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT:
                addRobotInCircle(position);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK:
                addPointToTrack(position);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
                break;
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE:
                addPointRobotsSize(position);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION:
            default:
                break;
        }; // switch
    }

    void ControlCenterGUI::on_cbDisplayRSSI_stateChanged(int state)
    {
        foreach(StatisticsDisplayer* statDisplayer, statisticsDisplayers)
        {
            statDisplayer->SetDisplayRSSI(Qt::Checked==state);
        }
    }

    void ControlCenterGUI::on_cbDisplayLQI_stateChanged(int state)
    {
        foreach(StatisticsDisplayer* statDisplayer, statisticsDisplayers)
        {
            statDisplayer->SetDisplayLQI(Qt::Checked==state);
        }
    }

    void ControlCenterGUI::on_cbDisplayDelay_stateChanged(int state)
    {
        foreach(StatisticsDisplayer* statDisplayer, statisticsDisplayers)
        {
            statDisplayer->SetDisplayDelay(Qt::Checked==state);
        }
    }

    void ControlCenterGUI::on_controlCenterDisplayer_doubleClicked(QPoint /*position*/)
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK:
                addTrack();
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT:
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE:
            default:
                break;
        }; // switch
    }

    void ControlCenterGUI::on_controlCenterDisplayer_mouseMoved(QPoint position)
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK:
                addTemporaryPointToTrack(position);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT:
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION:
            case CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE:
            default:
                break;
        }; // switch
    }

    void ControlCenterGUI::onLoadCalibration()
    {
        ///////// Selecting a file destination
        QString file = QFileDialog::getOpenFileName(calibrationWindow, QString::fromUtf8("Load from file"), QString::fromUtf8("c:\\"), QString::fromUtf8("XML files (*.xml)"));
        
        // Saving parameters
        if(!file.isEmpty())
            controlCenter->LoadCalibrationFromFile(file);
    }

    void ControlCenterGUI::onSaveCalibration()
    {
        ///////// Selecting a file destination
        QString file = QFileDialog::getSaveFileName(calibrationWindow, QString::fromUtf8("Save to file"), QString::fromUtf8("c:\\"), QString::fromUtf8("XML files (*.xml)"));
        
        // Saving parameters
        if(!file.isEmpty())
            controlCenter->SaveCalibrationToFile(file);
    }
    
    void ControlCenterGUI::startWindow()
    {
		// Just to be sure        
        controlCenterView->SetDisplayCirclesActivated(false);
        controlCenter->SetCalibrationActivated(false);
        controlCenter->SetNormalizationActivated(false);
        // Initialize timer
        connect(&timerFPS, SIGNAL(timeout()), this, SLOT(onTimerFPSTimeOut()));
        timerFPS.start(CC_CONTROL_CENTER_GUI_TIMER_FPS);
        // Initialize machine state
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_NORMAL);
        // Update some specifics fields
        QSize videoSourceSize(controlCenter->GetVideoSize());
        laWidth->setText(QString::number(videoSourceSize.width())+QString::fromUtf8("px"));
        laHeight->setText(QString::number(videoSourceSize.height())+QString::fromUtf8("px"));
        // Propage robots size (be sure that these points have been initialized)
        controlCenter->SetKheperaSize(QPoint(robotsSizePoint1-robotsSizePoint2).manhattanLength());
        ControlCenterView::SetRobotSize(QPoint(robotsSizePoint1-robotsSizePoint2).manhattanLength());
        RobotView::SetRobotSize(QPoint(robotsSizePoint1-robotsSizePoint2).manhattanLength());
        // Launch khepera processor images
        controlCenter->StartTrackingKheperaProcess();
        // Show the window
        show();
    }

    void ControlCenterGUI::onTimerFPSTimeOut()
    {
        laFPS->setText(QString::number(controlCenter->GetFPS()));
    }

    void ControlCenterGUI::OnNewPositionsFindCount(int count)
    {
        updatePositionsFindLabel(count);
    }

    void ControlCenterGUI::updatePositionsFindLabel(int count)
    {
        laPositionsFind->setText(QString::number(count));
    }

    void ControlCenterGUI::stopCalibrationWindow()
    {
        //////////// Save the image displayer
        mainLayout->insertWidget(0,controlCenterDisplayer);

        //////////// Disconnect signals
        disconnect(calibrationWindow, 0, 0, 0);
        disconnect(controlCenter, 0, calibrationWindow, 0);
        disconnect(this         , 0, calibrationWindow, 0);

        delete calibrationWindow;
        calibrationWindow = 0;
    }

    void ControlCenterGUI::onStartCalibration()
    {
        CalibrationWindow::ParamCalibration params = calibrationWindow->GetBoardParameters();
        controlCenter->InitializeUndistortionCalibration(params.boardWidth,params.boardHeight,params.boardCount);
        controlCenter->StartCalibration();
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_UNDISTORTION);
    }

    void ControlCenterGUI::onStopCalibration()
    {
        controlCenter->StopProcessing();
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL);
    }

    void ControlCenterGUI::onStartDefineRobotsSize()
    {
        controlCenterDisplayer->EnableTool();
        // Initialize
        robotsSizeLastPointClicked.setX(-1);robotsSizeLastPointClicked.setY(-1);
        // Update state
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_DEFINE_ROBOTS_SIZE);
    }

    void ControlCenterGUI::onStopDefineRobotsSize()
    {
        controlCenterDisplayer->DisableTool();
        // Update state
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL);
    }

    void ControlCenterGUI::addPointRobotsSize(const QPoint& position)
    {
        if(-1==robotsSizeLastPointClicked.x())
        {
            robotsSizeLastPointClicked = controlCenterView->ConvertCoordinateToVideoSourceSize(position);
        }
        else
        {
            robotsSizePoint1 = robotsSizeLastPointClicked;
            robotsSizePoint2 = controlCenterView->ConvertCoordinateToVideoSourceSize(position);
            controlCenterDisplayer->DisableTool();
            // Update state
            setWindowMode(CC_CONTROL_CENTER_GUI_MODE_CALIBRATION_NORMAL);
            // Signal that we have finished
            emit RobotsSizeDefined();
        }
    }


    void ControlCenterGUI::on_btnAddRobot_clicked()
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
                controlCenterDisplayer->EnableTool();
                setWindowMode(CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_ROBOT:
                controlCenterDisplayer->DisableTool();
                setWindowMode(CC_CONTROL_CENTER_GUI_MODE_NORMAL);
                break;
            default:
                break;
        };
    }

    void ControlCenterGUI::on_btnAddTrack_clicked()
    {
        switch(windowMode)
        {
            case CC_CONTROL_CENTER_GUI_MODE_NORMAL:
                tempTrack     = new Track(QString::fromUtf8("temp"));
                tempTrackView = new TrackView(tempTrack,false);
                controlCenterView->AddView(tempTrack->GetIdentifier(),tempTrackView);
                controlCenterDisplayer->EnableTool();
                controlCenterDisplayer->setMouseTracking(true);
                setWindowMode(CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK);
                break;
            case CC_CONTROL_CENTER_GUI_MODE_ADD_TRACK:
                disableAddTrackMode();
                break;
            default:
                break;
        };
    }

    void ControlCenterGUI::connectLayer(const Layer * layer)
    {
        connect(layer, SIGNAL(DisplayStateChanged(int,bool)),   controlCenterView,  SLOT(SetDisplayView(int,bool)));
        connect(layer, SIGNAL(DeleteObject(int,int)),           this,               SLOT(deleteObject(int,int)));
        connect(layer, SIGNAL(clicked(int,int)),                this,               SLOT(layerClicked(int,int)));
    }

    void ControlCenterGUI::disconnectLayer(const Layer * layer)
    {
        disconnect(layer, 0, 0, 0);
    }

    void ControlCenterGUI::disableAddTrackMode()
    {
        controlCenterDisplayer->DisableTool();
        controlCenterDisplayer->setMouseTracking(false);
        controlCenterView->RemoveView(tempTrack->GetIdentifier());
        delete tempTrackView; tempTrackView = 0;
        delete tempTrack; tempTrack = 0;
        setWindowMode(CC_CONTROL_CENTER_GUI_MODE_NORMAL);
    }


    void ControlCenterGUI::on_robotsController_Backward()
    {        
        foreach(Layer* layer, layers)
        {
            if(Layer::LT_ROBOT==layer->GetLayerType() && layer->IsSelected())
                controlCenter->MoveRobot(layer->GetIdentifier(),-robotsController->GetSpeed(),-robotsController->GetSpeed());
        }
    }

    void ControlCenterGUI::on_robotsController_Forward()
    {
        foreach(Layer* layer, layers)
        {
            if(Layer::LT_ROBOT==layer->GetLayerType() && layer->IsSelected())
                controlCenter->MoveRobot(layer->GetIdentifier(),robotsController->GetSpeed(),robotsController->GetSpeed());
        }
    }

    void ControlCenterGUI::on_robotsController_RotateLeft()
    {
        foreach(Layer* layer, layers)
        {
            if(Layer::LT_ROBOT==layer->GetLayerType() && layer->IsSelected())
                controlCenter->MoveRobot(layer->GetIdentifier(),-robotsController->GetSpeed(),robotsController->GetSpeed());
        }
    }

    void ControlCenterGUI::on_robotsController_RotateRight()
    {
        foreach(Layer* layer, layers)
        {
            if(Layer::LT_ROBOT==layer->GetLayerType() && layer->IsSelected())
                controlCenter->MoveRobot(layer->GetIdentifier(),robotsController->GetSpeed(),-robotsController->GetSpeed());
        }
    }

    void ControlCenterGUI::on_robotsController_Stop()
    {
        foreach(Layer* layer, layers)
        {
            if(Layer::LT_ROBOT==layer->GetLayerType() && layer->IsSelected())
                controlCenter->MoveRobot(layer->GetIdentifier(),CC_CONTROL_CENTER_GUI_SPEED_STOP,CC_CONTROL_CENTER_GUI_SPEED_STOP);
        }
    }

    void ControlCenterGUI::addRobotInCircle(const QPoint& position)
    {
        // Retrieve datas to create the robot
        RobotCreator::RobotCreationParameters params = robotCreator->GetParameters();
        if(params.name.isEmpty())
        {
            QMessageBox::warning(this,QString::fromUtf8("Add a robot"), QString::fromUtf8("Please fill the name field."));
            return;
        }
        if(Communicator::COMMUNICATOR_MODE_MULTI_ROBOT==params.communicatorMode && params.address.isEmpty())
        {
            QMessageBox::warning(this,QString::fromUtf8("Add a robot"), QString::fromUtf8("You have selected a multi-robot port. So, please fill the address field."));
            return;
        }

        if(controlCenter->PositionExists(controlCenterView->ConvertCoordinateToVideoSourceSize(position)))
        {
            // create the robot, add the view and add the new robot to layers
            const Robot* robot = controlCenter->AddKhepera(params.name,controlCenterView->ConvertCoordinateToVideoSourceSize(position),params.interfaceName,params.communicatorMode,params.address);    
            if(!robot)
            {
                QMessageBox::warning(this,QString::fromUtf8("Add a robot"),QString::fromUtf8("Mode selected for this COM port is incompatible with another robot.\nPlease select another one."));
                return;
            }
            controlCenterView->AddView(robot->GetIdentifier(),new RobotView(robot));
            RobotLayer* newRobotLayer   = new RobotLayer(robot);
            robotsLayers->addWidget(newRobotLayer);
            layers.insert(robot->GetIdentifier(),newRobotLayer);
            if(params.communicatorMode==Communicator::COMMUNICATOR_MODE_MULTI_ROBOT)
            {   
                StatisticsDisplayer* statDisplayer = new StatisticsDisplayer(robot);
                statDisplayer->SetDisplayDelay(cbDisplayDelay->isChecked());
                statDisplayer->SetDisplayLQI(cbDisplayLQI->isChecked());
                statDisplayer->SetDisplayRSSI(cbDisplayRSSI->isChecked());
                statsDisplayers->addWidget(statDisplayer);
                statisticsDisplayers.insert(robot->GetIdentifier(),statDisplayer);
                connect(controlCenter, SIGNAL(ImageProcessed()), statDisplayer, SLOT(update()));
            }

            // connect signals
            connectLayer(newRobotLayer);
            connect(newRobotLayer, SIGNAL(AddTrackRobotAssociationClicked(int)),    this, SLOT(onAddTrackRobotAssociationClicked(int)));
            connect(newRobotLayer, SIGNAL(RemoveTrackRobotAssociationClicked(int)), this, SLOT(onRemoveTrackRobotAssociationClicked(int)));
            connect(newRobotLayer, SIGNAL(StartTrackFollowerClicked(int)),          this, SLOT(onStartTrackFollowerClicked(int)));
            connect(newRobotLayer, SIGNAL(StopTrackFollowerClicked(int)),           this, SLOT(onStopTrackFollowerClicked(int)));
            connect(newRobotLayer, SIGNAL(PauseTrackFollowerClicked(int)),          this, SLOT(onPauseTrackFollowerClicked(int)));
            connect(newRobotLayer, SIGNAL(RepeatTrackFollowerClicked(int,bool)),    this, SLOT(onRepeatTrackFollowerClicked(int,bool)));
            connect(newRobotLayer, SIGNAL(RotationSpeedChanged(int,int)),           this, SLOT(onRotationSpeedChanged(int,int)));
            connect(newRobotLayer, SIGNAL(MainSpeedChanged(int,int)),               this, SLOT(onMainSpeedChanged(int,int)));
            
            // Update window
            setWindowMode(CC_CONTROL_CENTER_GUI_MODE_NORMAL);

            // Initialize robot Orientation
            initializeRobotOrientation(robot->GetIdentifier());
        }
    }

    void ControlCenterGUI::initializeRobotOrientation(int robotId)
    {
        const Robot* robot = controlCenter->GetRobot(robotId);
        if(robot)
        {
            connect(robot, SIGNAL(OrientationUpdated(int)), this, SLOT(OnOrientationUpdated(int)));
            controlCenter->MoveRobot(robotId, CC_CONTROL_CENTER_GUI_SPEED_INIT_ORIENTATION, CC_CONTROL_CENTER_GUI_SPEED_INIT_ORIENTATION);
        }
    }

    void ControlCenterGUI::OnOrientationUpdated(int robotId)
    {
        const Robot* robot = controlCenter->GetRobot(robotId);
        if(robot)
        {
            disconnect(robot, SIGNAL(OrientationUpdated(int)), this, SLOT(OnOrientationUpdated(int)));
            controlCenter->MoveRobot(robotId, CC_CONTROL_CENTER_GUI_SPEED_STOP, CC_CONTROL_CENTER_GUI_SPEED_STOP);
        }
    }

    void ControlCenterGUI::addTrack()
    {
        tempTrack->pop_back();
        if(trackCreator->GetParameters().name.isEmpty())
        {
            QMessageBox::warning(this,QString::fromUtf8("Add a track"),QString::fromUtf8("You must define a name for the track."));
            return;
        }
        if(tempTrack->size()<2)
        {
            QMessageBox::warning(this,QString::fromUtf8("Add a track"),QString::fromUtf8("You must select at least two points."));
            return;
        }
        
        const Track* t = controlCenter->AddTrack(trackCreator->GetParameters().name,tempTrack);
        controlCenterView->AddView(t->GetIdentifier(),new TrackView(t));

        TrackLayer* newTrackLayer   = new TrackLayer(t);
        tracksLayers->addWidget(newTrackLayer);
        layers.insert(t->GetIdentifier(),newTrackLayer);

        // connect signals
        connectLayer(newTrackLayer);

        // Update window
        disableAddTrackMode();
    }

    void ControlCenterGUI::addTemporaryPointToTrack(const QPoint& point)
    {
        if(!controlCenterView->IsPointInImage(point)) return;
        if(tempTrack->size()<1) return;
        if(tempTrack->size()>1) tempTrack->pop_back();
        tempTrack->append(controlCenterView->ConvertCoordinateToVideoSourceSize(point));
        //controlCenterDisplayer->update();
    }

    void ControlCenterGUI::addPointToTrack(const QPoint& point)
    {
        if(!controlCenterView->IsPointInImage(point)) return;
        tempTrack->append(controlCenterView->ConvertCoordinateToVideoSourceSize(point));
    }

    void ControlCenterGUI::deleteObject(int layerType, int objectIdentifier)
    {
        switch(layerType)
        {
            case Layer::LT_ROBOT:
                removeRobot(objectIdentifier);
                break;
            case Layer::LT_TRACK:
                removeTrack(objectIdentifier);
                break;
            default:
                return;
        }; // switch
    }

    void ControlCenterGUI::layerClicked(int layerType, int objectIdentifier)
    {
        switch(layerType)
        {
            case Layer::LT_ROBOT:
            case Layer::LT_TRACK:
                layers[objectIdentifier]->SetSelected(!layers[objectIdentifier]->IsSelected());
                break;
            default:
                return;
        }; // switch
    }

    void ControlCenterGUI::removeRobot(int identifier)
    {
        // Disconnect
            // TODO
        // Delete Layer
        foreach(Layer* layer, layers)
        {
            if(layer->GetIdentifier()==identifier)
            {
                layers.remove(identifier);
                statisticsDisplayers.remove(identifier);
                robotsLayers->removeWidget(layer);
                disconnectLayer(layer);
                delete layer; layer = 0;
                break;
            }
        } // foreach
        // Delete View
        controlCenterView->RemoveView(identifier);
        // Delete Object
        controlCenter->RemoveRobot(identifier);
    }

    void ControlCenterGUI::removeTrack(int identifier)
    {
        // Disconnect
            // TODO
        // Delete Layer
        foreach(Layer* layer, layers)
        {
            if(layer->GetIdentifier()==identifier)
            {
                layers.remove(identifier);
                tracksLayers->removeWidget(layer);
                disconnectLayer(layer);
                delete layer; layer = 0;
                break;
            }
        } // foreach
        // Delete View
        controlCenterView->RemoveView(identifier);
        // Update robots layers (for robots associated with the the track trackId)
        QList<int> associatedRobots = controlCenter->GetAssociatedRobotsToTrack(identifier);
        foreach(int robotId, associatedRobots)
        {
            if(layers.contains(robotId) && Layer::LT_ROBOT==layers[robotId]->GetLayerType()) 
            {
                layers[robotId]->SetParameter(RobotLayer::PID_HAS_TRACK_ASSOCIATION,false);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_STOPPED);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Stopped"));
                layers[robotId]->SetParameter(RobotLayer::PID_REPEAT,false);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_NAME,QString::fromUtf8("None"));
            }
        }
        // Delete Object
        controlCenter->RemoveTrack(identifier);
    }

    void ControlCenterGUI::onAddTrackRobotAssociationClicked(int robotId)
    {
        // Just some verification
        if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

        // Search for the first Track selected
        QMap<int, Layer*>::iterator it;
        for(it = layers.begin(); it!=layers.end();++it)
        {
            Layer* layer = it.value();
            if(Layer::LT_TRACK==layer->GetLayerType() && layer->IsSelected())
            {
                controlCenter->AddRobotToFollowTrack(robotId,layer->GetIdentifier(),CC_CONTROL_CENTER_GUI_DEFAULT_LAPS_COUNT);
                layers[robotId]->SetParameter(RobotLayer::PID_HAS_TRACK_ASSOCIATION,true);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_STOPPED);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Stopped"));
                layers[robotId]->SetParameter(RobotLayer::PID_REPEAT,false);
                layers[robotId]->SetParameter(RobotLayer::PID_TRACK_NAME,controlCenter->GetTrackName(layer->GetIdentifier()));
                statusBar()->clearMessage();
                return;
            }
        }

        // If we are here it symply means that the user didn't select a track
        statusBar()->showMessage(QString::fromUtf8("Please, select a track !"), 3000);
    }

    void ControlCenterGUI::onRemoveTrackRobotAssociationClicked(int robotId)
    {
        if(controlCenter->RemoveRobotTrackAssociation(robotId))
        {
            if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

            layers[robotId]->SetParameter(RobotLayer::PID_HAS_TRACK_ASSOCIATION,false);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_STOPPED);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Stopped"));
            layers[robotId]->SetParameter(RobotLayer::PID_REPEAT,false);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_NAME,QString::fromUtf8("None"));
        }
    }

    void ControlCenterGUI::onStartTrackFollowerClicked(int robotId)
    {
        if(controlCenter->StartRobotFollowingTrack(robotId))
        {
            if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_RUNNING);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Running"));
        }
    }

    void ControlCenterGUI::onStopTrackFollowerClicked(int robotId)
    {
        if(controlCenter->StopRobotFollowingTrack(robotId))
        {
            if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_STOPPED);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Stopped"));
        }
    }

    void ControlCenterGUI::onPauseTrackFollowerClicked(int robotId)
    {
        if(controlCenter->PauseRobotFollowingTrack(robotId))
        {
            if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_PAUSED);
            layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Paused"));
        }
    }

    void ControlCenterGUI::onRepeatTrackFollowerClicked(int robotId, bool activated)
    {
        int value = 1;
        if(activated) value = ControlCenter::CC_CONTROL_CENTER_INFINITE_LAPS;
        if(controlCenter->DefineTrackFollowingLapsTodo(robotId, value))
        {
            if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

            layers[robotId]->SetParameter(RobotLayer::PID_REPEAT,activated);
        }
    }

    void ControlCenterGUI::onEndOfTrackReached(int robotId)
    {
        if(!layers.contains(robotId) || Layer::LT_ROBOT!=layers[robotId]->GetLayerType()) return;

        layers[robotId]->SetParameter(RobotLayer::PID_TRACK_FOLLOWER_STATE,RobotLayer::TF_STOPPED);
        layers[robotId]->SetParameter(RobotLayer::PID_TRACK_STRING_STATE,QString::fromUtf8("Terminated !"));

    }

    void ControlCenterGUI::onRotationSpeedChanged(int robotId, int value)
    {
        controlCenter->SetRobotRotationSpeed(robotId,value);
    }

    void ControlCenterGUI::onMainSpeedChanged(int robotId, int value)
    {
        controlCenter->SetRobotMainSpeed(robotId,value);
    }

    void ControlCenterGUI::setupFileMenu()
    {
        QMenu* fileMenu = new QMenu(QString::fromUtf8("&File"),this);

        menuBar()->addMenu(fileMenu);

        fileMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/quit.png")),QString::fromUtf8("&Quit"), this, SLOT(close()), QKeySequence::Close);
    }

    void ControlCenterGUI::setupDisplayMenu()
    {
        QMenu* displayMenu = new QMenu(QString::fromUtf8("&Display"),this);

        menuBar()->addMenu(displayMenu);

        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/robot.png")),QString::fromUtf8("Show / Hide &robot panel"), this, SLOT(onShowHideRobotPanelActionClicked()));
        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/track.png")),QString::fromUtf8("Show / Hide &track panel"), this, SLOT(onShowHideTrackPanelActionClicked()));
        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/histogram.png")),QString::fromUtf8("Show / Hide &histogram"), this, SLOT(onShowHideHistogramActionClicked()));
        
        displayMenu->addSeparator();
        
        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/calibrate.png")),QString::fromUtf8("&Calibrate image"), this, SLOT(onCalibrateActionClicked()));
        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/normalize.png")),QString::fromUtf8("&Normalize image histogram"), this, SLOT(onNormalizeActionClicked()));
        displayMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/display.png")),QString::fromUtf8("&Display image"), this, SLOT(onDisplayActionClicked()));
    }

    void ControlCenterGUI::setupHelpMenu()
    {
        QMenu* helpMenu = new QMenu(QString::fromUtf8("&Help"),this);

        menuBar()->addMenu(helpMenu);

        helpMenu->addAction(QIcon(QString::fromUtf8(":/icons/ressources/information.png")),QString::fromUtf8("&Help"), this, SLOT(onHelpActionClicked()), QKeySequence::HelpContents);
    }

    void ControlCenterGUI::onDisplayActionClicked()
    {
        cbDisplayImage->setChecked(!cbDisplayImage->isChecked());
    }

    void ControlCenterGUI::onCalibrateActionClicked()   
    {
        cbCalibrateImage->setChecked(!cbCalibrateImage->isChecked());
    }

    void ControlCenterGUI::onNormalizeActionClicked()   
    {
        cbNormalizeImage->setChecked(!cbNormalizeImage->isChecked());
    }

    void ControlCenterGUI::onHelpActionClicked()
    {
        QMessageBox::information(this,QString::fromUtf8("Help"),QString::fromUtf8("Sorry ... not done ... not yet !"));
    }

    void ControlCenterGUI::onShowHideRobotPanelActionClicked()
    {
        robotsDock->setVisible(!robotsDock->isVisible());
    }

    void ControlCenterGUI::onShowHideTrackPanelActionClicked()
    {
        tracksDock->setVisible(!tracksDock->isVisible());
    }

    void ControlCenterGUI::onShowHideHistogramActionClicked()
    {
        histogramDisplayer->setVisible(!histogramDisplayer->isVisible());
    }
} // namespace CCF