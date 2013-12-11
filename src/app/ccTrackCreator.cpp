#include "ccTrackCreator.h"

#include <QtGui/QLabel>

#include <QtCore/QString>
#include <QtCore/QMetaObject>


namespace CCF
{
    TrackCreator::TrackCreator(QWidget* parent)
        :QWidget(parent)
    {
        setupInterface();       
    }

    TrackCreator::TrackCreationParameters TrackCreator::GetParameters() const
    {
        TrackCreationParameters params;
        params.name = edName->text();

        return params;
    }

    void TrackCreator::setupInterface()
    {
        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->addWidget(new QLabel(QString::fromUtf8("Name :"),this));
            edName = new QLineEdit(this);
            edName->setObjectName(QString::fromUtf8("edName"));
        mainLayout->addWidget(edName);

        QMetaObject::connectSlotsByName(this);
    }

    void TrackCreator::setVisible(bool visible)
    {
        QWidget::setVisible(visible);
        if(visible)
        {
            edName->clear();
            edName->setFocus();
        }
    }

} // namespace CCF