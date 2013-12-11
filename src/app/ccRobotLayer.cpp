#include "ccRobotLayer.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QFont>
#include <QtGui/QSizePolicy>
#include <QtGui/QSpacerItem>

#include <QtCore/QRegExp>
#include <QtCore/QMetaObject>


namespace CCF
{
    static const double CC_ROBOT_LAYER_PI   = 3.14159265;

    QString RobotLayer::formatPortName(const QString& name)
    {
        QRegExp rx("(\\d+)");
        QString formatedName(name);
        if(formatedName.contains(rx)) {
            int portNum = rx.cap(1).toInt();
            formatedName = "COM " + QString::number(portNum);
        }
        return formatedName;
    }

    RobotLayer::RobotLayer(const Displayable* _object, QWidget* parent)
        :Layer(_object,parent)
    {
        // Initialize widgets
        setupSpecificInterface();
        hasTrackAssociated = false;
        repeat = false;
        trackFollowerState = TF_STOPPED;
        laPort->setText(formatPortName(robot()->GetInterfaceName()));
        slRotationSpeed->setValue(robot()->GetUserRotationSpeed());
        slMovementSpeed->setValue(robot()->GetUserSpeed());
        UpdatePosition();
        UpdateOrientation();
        updateInterface();

        // Connect signals
        connect((const QObject*)object, SIGNAL(PositionUpdated(int)),    this, SLOT(UpdatePosition()));
        connect((const QObject*)object, SIGNAL(OrientationUpdated(int)), this, SLOT(UpdateOrientation()));

        // Display the widget
        show();
    }

    Layer::LayerType RobotLayer::GetLayerType() const
    {
        return LT_ROBOT;
    }

    void RobotLayer::SetHasTrackAssociated(bool value)
    {
        hasTrackAssociated = value;
        updateInterface();
    }

    bool RobotLayer::SetParameter(int parameterId, int value)
    {
        switch(parameterId)
        {
            case PID_TRACK_FOLLOWER_STATE:
                SetTrackFollowerState((TrackFollowerState)value);
                return true;
            default:
                return false;
        };
    }
    
    bool RobotLayer::SetParameter(int parameterId, bool value)
    {
        switch(parameterId)
        {
            case PID_HAS_TRACK_ASSOCIATION:
                SetHasTrackAssociated(value);
                return true;
            case PID_REPEAT:
                SetRepeat(value);
                return true;
            default:
                return false;
        };
    }

    bool RobotLayer::SetParameter(int parameterId, const QString& value)
    {
        switch(parameterId)
        {
            case PID_TRACK_NAME:
                SetTrackName(value);
                return true;
            case PID_TRACK_STRING_STATE:
                SetStringState(value);
                return true;
            default:
                return false;
        };
    }

    void RobotLayer::SetTrackName(const QString& value)
    {
        laTrackName->setText(value);
    }

    void RobotLayer::SetStringState(const QString& value)
    {
        laTrackState->setText(value);
    }

    void RobotLayer::SetTrackFollowerState(TrackFollowerState state)
    {
        trackFollowerState = state;
        updateInterface();
    }

    void RobotLayer::SetRepeat(bool value)
    {
        repeat = value;
        updateInterface();
    }

    void RobotLayer::UpdatePosition()
    {
        //robot()->lock();
        laPositionX->setText(QString::number(robot()->GetPosition().x()));
        laPositionY->setText(QString::number(robot()->GetPosition().y()));
        //robot()->unlock();
    }

    void RobotLayer::UpdateOrientation()
    {
        //robot()->lock();
        laOrientation->setText(QString::number((int)(robot()->GetOrientation()*180/CC_ROBOT_LAYER_PI)+180));
        //robot()->unlock();
    }

    const Robot* RobotLayer::robot() const
    {
        return (const Robot*)object;
    }

    void RobotLayer::updateInterface()
    {
        btnRemoveTrack->setVisible(hasTrackAssociated);
        btnNewTrack->setVisible(!hasTrackAssociated);
        btnPauseFollowTrack->setEnabled(hasTrackAssociated && TF_RUNNING==trackFollowerState);
        btnStopFollowTrack->setEnabled(hasTrackAssociated  && TF_STOPPED!=trackFollowerState);
        btnStartFollowTrack->setEnabled(hasTrackAssociated && TF_RUNNING!=trackFollowerState);
        btnRepeatFollowTrack->setEnabled(hasTrackAssociated);
        if(repeat)
        {
            btnRepeatFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnRepeatFollowTrack\n"
                "{	\n"
                "	background-image: url(:/icons/ressources/repeat_activated.png);\n"
                "    margin: 0;\n"
                "    padding: 0;\n"
                "    border: none;\n"
                "}\n"
                "\n"
                "QPushButton#btnRepeatFollowTrack:hover\n"
                "{	\n"
                "	background-image: url(:/icons/ressources/repeat_light.png);\n"
                "}"));
        }
        else
        {
            btnRepeatFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnRepeatFollowTrack\n"
                "{	\n"
                "	background-image: url(:/icons/ressources/repeat.png);\n"
                "    margin: 0;\n"
                "    padding: 0;\n"
                "    border: none;\n"
                "}\n"
                "\n"
                "QPushButton#btnRepeatFollowTrack:hover\n"
                "{	\n"
                "	background-image: url(:/icons/ressources/repeat_light.png);\n"
                "}"));
        }
    }

    void RobotLayer::setupSpecificInterface()
    {
        btnDeleteObject->setStyleSheet(QString::fromUtf8("QPushButton#btnDeleteObject\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/robot_delete.png);\n"
                    "    margin: 0;\n"
                    "    padding: 0;\n"
                    "    border: none;\n"
                    "}\n"
                    "\n"
                    "QPushButton#btnDeleteObject:hover\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/robot_delete_light.png);\n"
                    "}"));

        setObjectName(QString::fromUtf8("robotLayer"));
        setStyleSheet(QString::fromUtf8("QWidget#robotLayer\n"
                            "{	\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "}"));
        contentLayout = new QVBoxLayout();
            QHBoxLayout* temp = new QHBoxLayout();
                temp->addWidget(new QLabel(QString::fromUtf8("Interface name :"),this));
                laPort = new QLabel(this);
            temp->addWidget(laPort);
            temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum));
        static_cast<QVBoxLayout*>(contentLayout)->addLayout(temp);
            temp = new QHBoxLayout();
            temp->addWidget(new QLabel(QString::fromUtf8("X :"),this));
                laPositionX = new QLabel(this);
            temp->addWidget(laPositionX);
            temp->addWidget(new QLabel(QString::fromUtf8("Y :"),this));
                laPositionY = new QLabel(this);
            temp->addWidget(laPositionY);
            temp->addWidget(new QLabel(QString::fromUtf8("Theta :"),this));
                laOrientation = new QLabel(this);
            temp->addWidget(laOrientation);
            temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum));
        static_cast<QVBoxLayout*>(contentLayout)->addLayout(temp);
            QGroupBox* tempGb = new QGroupBox(QString::fromUtf8("Track follower"));
                QVBoxLayout* tempGbLayout = new QVBoxLayout(tempGb);
                    temp = new QHBoxLayout();
                        laTrackName = new QLabel(QString::fromUtf8("None"),tempGb);
                        QFont trackNameFont;
                        trackNameFont.setItalic(true);
                        laTrackName->setFont(trackNameFont);
                    temp->addWidget(laTrackName);
                    temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum));
                        btnNewTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnNewTrack->setObjectName(QString::fromUtf8("btnNewTrack"));
                        btnNewTrack->setFixedSize(16,16);
                        btnNewTrack->setFocusPolicy(Qt::ClickFocus);
                        btnNewTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnNewTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/follow_track.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnNewTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/follow_track_light.png);\n"
                            "}"));
                        btnNewTrack->setToolTip(QString::fromUtf8("Follow the selected track"));
                    temp->addWidget(btnNewTrack);
                        btnRemoveTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnRemoveTrack->setObjectName(QString::fromUtf8("btnRemoveTrack"));
                        btnRemoveTrack->setFixedSize(16,16);
                        btnRemoveTrack->setFocusPolicy(Qt::ClickFocus);
                        btnRemoveTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnRemoveTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/remove_followed_track.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnRemoveTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/remove_followed_track_light.png);\n"
                            "}"));
                        btnRemoveTrack->setToolTip(QString::fromUtf8("Forget it !"));
                        btnRemoveTrack->setVisible(false);
                    temp->addWidget(btnRemoveTrack);
                tempGbLayout->addLayout(temp);
                    temp = new QHBoxLayout();
                    temp->addWidget(new QLabel(QString::fromUtf8("State :"),tempGb));
                        laTrackState = new QLabel(QString::fromUtf8("none"),tempGb);
                    temp->addWidget(laTrackState);
                    temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum));
                        btnStartFollowTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnStartFollowTrack->setObjectName(QString::fromUtf8("btnStartFollowTrack"));
                        btnStartFollowTrack->setFixedSize(16,16);
                        btnStartFollowTrack->setFocusPolicy(Qt::ClickFocus);
                        btnStartFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnStartFollowTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/start.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnStartFollowTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/start_light.png);\n"
                            "}"));
                        btnStartFollowTrack->setToolTip(QString::fromUtf8("Start"));
                    temp->addWidget(btnStartFollowTrack);
                        btnStopFollowTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnStopFollowTrack->setObjectName(QString::fromUtf8("btnStopFollowTrack"));
                        btnStopFollowTrack->setFixedSize(16,16);
                        btnStopFollowTrack->setFocusPolicy(Qt::ClickFocus);
                        btnStopFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnStopFollowTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/stop.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnStopFollowTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/stop_light.png);\n"
                            "}"));
                        btnStopFollowTrack->setToolTip(QString::fromUtf8("Stop"));
                    temp->addWidget(btnStopFollowTrack);
                        btnPauseFollowTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnPauseFollowTrack->setObjectName(QString::fromUtf8("btnPauseFollowTrack"));
                        btnPauseFollowTrack->setFixedSize(16,16);
                        btnPauseFollowTrack->setFocusPolicy(Qt::ClickFocus);
                        btnPauseFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnPauseFollowTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/pause.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnPauseFollowTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/pause_light.png);\n"
                            "}"));
                        btnPauseFollowTrack->setToolTip(QString::fromUtf8("Pause"));
                    temp->addWidget(btnPauseFollowTrack);
                        btnRepeatFollowTrack = new QPushButton(QString::fromUtf8(""),tempGb);
                        btnRepeatFollowTrack->setObjectName(QString::fromUtf8("btnRepeatFollowTrack"));
                        btnRepeatFollowTrack->setFixedSize(16,16);
                        btnRepeatFollowTrack->setFocusPolicy(Qt::ClickFocus);
                        btnRepeatFollowTrack->setStyleSheet(QString::fromUtf8("QPushButton#btnRepeatFollowTrack\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/repeat.png);\n"
                            "    margin: 0;\n"
                            "    padding: 0;\n"
                            "    border: none;\n"
                            "}\n"
                            "\n"
                            "QPushButton#btnRepeatFollowTrack:hover\n"
                            "{	\n"
                            "	background-image: url(:/icons/ressources/repeat_light.png);\n"
                            "}"));
                        btnRepeatFollowTrack->setToolTip(QString::fromUtf8("Repeat"));
                    temp->addWidget(btnRepeatFollowTrack);
                tempGbLayout->addLayout(temp);
                    temp = new QHBoxLayout();
                    temp->addWidget(new QLabel(QString::fromUtf8("Rotation speed"),tempGb));
                    temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum));
                        slRotationSpeed = new QSlider(Qt::Horizontal,tempGb);
                        slRotationSpeed->setObjectName(QString::fromUtf8("slRotationSpeed"));
                        slRotationSpeed->setRange(0,20);
                        slRotationSpeed->setSingleStep(1);
                    temp->addWidget(slRotationSpeed);
                tempGbLayout->addLayout(temp);
                    temp = new QHBoxLayout();
                    temp->addWidget(new QLabel(QString::fromUtf8("Main speed"),tempGb));
                    temp->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum));
                        slMovementSpeed = new QSlider(Qt::Horizontal,tempGb);
                        slMovementSpeed->setObjectName(QString::fromUtf8("slMovementSpeed"));
                        slMovementSpeed->setRange(0,20);
                        slMovementSpeed->setSingleStep(1);
                    temp->addWidget(slMovementSpeed);
                tempGbLayout->addLayout(temp);
        static_cast<QVBoxLayout*>(contentLayout)->addWidget(tempGb);

        mainLayout->addLayout(contentLayout);

        setFixedSize(220,200);
        QMetaObject::connectSlotsByName(this);
    }

    void RobotLayer::on_btnRemoveTrack_clicked()
    {
        emit RemoveTrackRobotAssociationClicked(GetIdentifier());
    }

    void RobotLayer::on_btnNewTrack_clicked()
    {
        emit AddTrackRobotAssociationClicked(GetIdentifier());
    }

    void RobotLayer::on_btnStartFollowTrack_clicked()
    {
        emit StartTrackFollowerClicked(GetIdentifier());
    }

    void RobotLayer::on_btnStopFollowTrack_clicked()
    {
        emit StopTrackFollowerClicked(GetIdentifier());
    }

    void RobotLayer::on_btnPauseFollowTrack_clicked()
    {
        emit PauseTrackFollowerClicked(GetIdentifier());
    }

    void RobotLayer::on_btnRepeatFollowTrack_clicked()
    {
        emit RepeatTrackFollowerClicked(GetIdentifier(),!repeat);
    }

    void RobotLayer::on_slRotationSpeed_valueChanged(int value)
    {
        emit RotationSpeedChanged(GetIdentifier(),value);
    }

    void RobotLayer::on_slMovementSpeed_valueChanged(int value)
    {
        emit MainSpeedChanged(GetIdentifier(),value);
    }
} // namespace CCF