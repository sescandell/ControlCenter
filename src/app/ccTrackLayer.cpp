#include "ccTrackLayer.h"

#include <QString>




namespace CCF
{
    TrackLayer::TrackLayer(const Displayable* _object, QWidget* parent)
        :Layer(_object,parent)
    {
        // Initialize widget informations
        setupSpecificInterface();
        cbDisplay->setText(track()->GetName());
        laPointsCount->setText(QString::number(track()->size()));

        // Display the widget
        show();
    }

    const Track* TrackLayer::track() const
    {
        return (const Track*)object;
    }

    Layer::LayerType TrackLayer::GetLayerType() const
    {
        return LT_TRACK;
    }

    void TrackLayer::setupSpecificInterface()
    {
        btnDeleteObject->setStyleSheet(QString::fromUtf8("QPushButton#btnDeleteObject\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/track_delete.png);\n"
                    "    margin: 0;\n"
                    "    padding: 0;\n"
                    "    border: none;\n"
                    "}\n"
                    "\n"
                    "QPushButton#btnDeleteObject:hover\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/track_delete_light.png);\n"
                    "}"));
        contentLayout = new QHBoxLayout();
        contentLayout->addWidget(new QLabel(QString::fromUtf8("Points count :"),this));
        laPointsCount = new QLabel(this);
        contentLayout->addWidget(laPointsCount);
        mainLayout->addLayout(contentLayout);
        setMaximumHeight(55);
        setMinimumHeight(55);
    }

} // namespace CCF