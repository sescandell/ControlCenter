#include "ccRobotController.h"

#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QLabel>

#include <QtCore/QMetaObject>



namespace CCF
{    
    RobotController::RobotController(QWidget* parent)
        :QWidget(parent)
    {
        setupInterface();

        connect(btnForward,     SIGNAL(pressed()), this, SIGNAL(Forward()), Qt::DirectConnection);
        connect(btnBackward,    SIGNAL(pressed()), this, SIGNAL(Backward()), Qt::DirectConnection);
        connect(btnRotateLeft,  SIGNAL(pressed()), this, SIGNAL(RotateLeft()), Qt::DirectConnection);
        connect(btnRotateRight, SIGNAL(pressed()), this, SIGNAL(RotateRight()), Qt::DirectConnection);

        connect(btnForward,     SIGNAL(released()), this, SIGNAL(Stop()), Qt::DirectConnection);
        connect(btnBackward,    SIGNAL(released()), this, SIGNAL(Stop()), Qt::DirectConnection);
        connect(btnRotateLeft,  SIGNAL(released()), this, SIGNAL(Stop()), Qt::DirectConnection);
        connect(btnRotateRight, SIGNAL(released()), this, SIGNAL(Stop()), Qt::DirectConnection);

        speed = slSpeed->value();
    }

    int RobotController::GetSpeed() const
    {
        return speed;
    }

    void RobotController::setupInterface()
    {
        QGridLayout* mainLayout = new QGridLayout(this);

        btnForward     = new QPushButton(QString::fromUtf8(""),this);
        btnForward->setObjectName(QString::fromUtf8("btnForward"));
        btnForward->setToolTip(QString::fromUtf8("Go forward"));
        btnForward->setFixedSize(23,51);
        btnForward->setStyleSheet(QString::fromUtf8("QPushButton#btnForward\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/forward.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnForward:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/forward_light.png);\n"
            "}"));
        btnBackward    = new QPushButton(QString::fromUtf8(""),this);
        btnBackward->setObjectName(QString::fromUtf8("btnBackward"));
        btnBackward->setToolTip(QString::fromUtf8("Go Backward"));
        btnBackward->setFixedSize(23,51);
        btnBackward->setStyleSheet(QString::fromUtf8("QPushButton#btnBackward\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/backward.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnBackward:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/backward_light.png);\n"
            "}"));
        btnRotateLeft  = new QPushButton(QString::fromUtf8(""),this);
        btnRotateLeft->setObjectName(QString::fromUtf8("btnRotateLeft"));
        btnRotateLeft->setToolTip(QString::fromUtf8("Rotate left"));
        btnRotateLeft->setFixedSize(23,53);
        btnRotateLeft->setStyleSheet(QString::fromUtf8("QPushButton#btnRotateLeft\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/rotate_left.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnRotateLeft:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/rotate_left_light.png);\n"
            "}"));
        btnRotateRight = new QPushButton(QString::fromUtf8(""),this);
        btnRotateRight->setToolTip(QString::fromUtf8("Rotate right"));
        btnRotateRight->setFixedSize(23,53);
        btnRotateRight->setObjectName(QString::fromUtf8("btnRotateRight"));
        btnRotateRight->setStyleSheet(QString::fromUtf8("QPushButton#btnRotateRight\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/rotate_right.png);\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "    border: none;\n"
            "}\n"
            "\n"
            "QPushButton#btnRotateRight:hover\n"
            "{	\n"
            "	background-image: url(:/icons/ressources/rotate_right_light.png);\n"
            "}"));

        mainLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum),0,0,2,1);
        mainLayout->addWidget(btnRotateLeft,0,1,2,1);
        mainLayout->addWidget(btnForward,0,2,1,1);
        mainLayout->addWidget(btnBackward,1,2,1,1);
        mainLayout->addWidget(btnRotateRight,0,3,2,1);
        mainLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Minimum),0,4,2,1);

        mainLayout->addWidget(new QLabel(QString::fromUtf8("Speed :"),this),2,0,1,1);
        slSpeed = new QSlider(Qt::Horizontal,this);
        slSpeed->setObjectName(QString::fromUtf8("slSpeed"));
        slSpeed->setRange(0,50);
        slSpeed->setValue(10);
        mainLayout->addWidget(slSpeed,2,1,1,3);

        QMetaObject::connectSlotsByName(this);
    }

    void RobotController::on_slSpeed_valueChanged(int value)
    {
        speed = value;
    }
}