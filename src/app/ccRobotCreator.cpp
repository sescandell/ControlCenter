#include "ccRobotCreator.h"

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QMetaObject>

#include <qextserialenumerator.h>

namespace CCF
{
    QString RobotCreator::formatPortName(const QString& name)
    {
        QRegExp rx("(\\d+)");
        QString formatedName(name);
        if(formatedName.contains(rx)) {
            int portNum = rx.cap(1).toInt();
            formatedName = "COM " + QString::number(portNum);
        }
        return formatedName;
    }
    
    RobotCreator::RobotCreator(QWidget* parent)
        :QWidget(parent)
    {
        setupInterface();

        QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
        foreach(QextPortInfo port, ports)
        {
            portMapping.insert(formatPortName(port.portName),port.portName);
            cbPort->addItem(formatPortName(port.portName));
        }
    }

    RobotCreator::RobotCreationParameters RobotCreator::GetParameters() const
    {
        RobotCreationParameters params;
        params.address          = edAddress->text();
        params.communicatorMode = gbMultiRobot->isChecked() ? Communicator::COMMUNICATOR_MODE_MULTI_ROBOT : Communicator::COMMUNICATOR_MODE_SINGLE_ROBOT;
        params.interfaceName    = portMapping[cbPort->currentText()];
        params.name             = edName->text();

        return params;
    }

    void RobotCreator::setVisible(bool visible)
    {
        QWidget::setVisible(visible);
        if(visible)
        {
            edName->clear();
            edName->setFocus();
            edAddress->clear();
        }
    }

    void RobotCreator::setupInterface()
    {
        QFormLayout* mainLayout = new QFormLayout(this);
        mainLayout->setWidget(0,QFormLayout::LabelRole, new QLabel(QString::fromUtf8("Name :"),this));
            edName = new QLineEdit(this);
            edName->setObjectName(QString::fromUtf8("edName"));
        mainLayout->setWidget(0,QFormLayout::FieldRole,edName);
        mainLayout->setWidget(1,QFormLayout::LabelRole, new QLabel(QString::fromUtf8("Port :"),this));
            cbPort = new QComboBox(this);
            cbPort->setEditable(false);
            cbPort->setObjectName(QString::fromUtf8("cbPort"));
        mainLayout->setWidget(1,QFormLayout::FieldRole,cbPort);
            gbMultiRobot = new QGroupBox(QString::fromUtf8("Multi-robot"),this);
            gbMultiRobot->setObjectName(QString::fromUtf8("gbMultiRobot"));
            gbMultiRobot->setCheckable(true);
            gbMultiRobot->setChecked(false);
                QHBoxLayout* tempLayout = new QHBoxLayout(gbMultiRobot);
                tempLayout->addWidget(new QLabel(QString::fromUtf8("Address :"),this));
                    edAddress = new QLineEdit(this);
                    edAddress->setObjectName(QString::fromUtf8("edAddress"));
                tempLayout->addWidget(edAddress);
        mainLayout->setWidget(2, QFormLayout::SpanningRole, gbMultiRobot);

        QMetaObject::connectSlotsByName(this);
    }

} // namespace CCF