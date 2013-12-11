#include "ccLayer.h"

#include <QtGui/QSpacerItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QFont>
#include <QtGui/QStyleOption>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtCore/QMetaObject>

namespace CCF
{
    static const int    CC_LAYER_RECTANGLE_ROUND_SIZE   = 5;
    static const QBrush CC_LAYER_SELECTED_BRUSH         = QBrush(QColor(220,220,220));
    static const QPen   CC_LAYER_SELECTED_PEN           = Qt::NoPen;
    static const QBrush CC_LAYER_BORDER_BRUSH           = Qt::NoBrush;
    static const QPen   CC_LAYER_BORDER_PEN             = QPen(QColor(200,200,200));


    Layer::Layer(const Displayable* _object, QWidget* parent)
        :QWidget(parent), object(_object), contentLayout(0)
    {
        setupInterface();
        setAutoFillBackground(true);
        SetSelected(false);
        setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum));
    }

    Layer::~Layer()
    {}

    Layer::LayerType Layer::GetLayerType() const
    {
        return LT_NONE;
    }

    int Layer::GetIdentifier() const
    {
        return object->GetIdentifier();
    }

    bool Layer::IsSelected() const
    {
        return selected;
    }

    void Layer::SetSelected(bool value)
    {
        selected = value;
        update();
    }

    void Layer::on_cbDisplay_stateChanged(int value)
    {
        emit DisplayStateChanged(GetIdentifier(),Qt::Checked==value);
    }

    void Layer::on_btnDeleteObject_clicked()
    {
        emit DeleteObject(GetLayerType(),GetIdentifier());
    }

    void Layer::mousePressEvent(QMouseEvent*)
    {
        emit clicked(GetLayerType(),GetIdentifier());
    }

    void Layer::paintEvent(QPaintEvent *ev)
     {
         QPainter p(this);

         
         if(selected)
         {
             p.setBrush(CC_LAYER_SELECTED_BRUSH);
             p.setPen(CC_LAYER_SELECTED_PEN);
             p.drawRoundRect(0,0,width()-1,height()-1,CC_LAYER_RECTANGLE_ROUND_SIZE,CC_LAYER_RECTANGLE_ROUND_SIZE);
         }

         p.setBrush(CC_LAYER_BORDER_BRUSH);
         p.setPen(CC_LAYER_BORDER_PEN);
         p.drawRoundRect(0,0,width()-1,height()-1,CC_LAYER_RECTANGLE_ROUND_SIZE,CC_LAYER_RECTANGLE_ROUND_SIZE);

         ev->accept();
     }
     

    void Layer::setupInterface()
    {
        mainLayout = new QVBoxLayout(this);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
            headerLayout = new QHBoxLayout();
                cbDisplay  = new QCheckBox(this);
                    cbDisplay->setObjectName(QString::fromUtf8("cbDisplay"));
                    cbDisplay->setText(object->GetName());
                    cbDisplay->setChecked(true);
                    QFont nameFont;
                    nameFont.setBold(true);
                    nameFont.setWeight(75);
                    cbDisplay->setFont(nameFont);
                headerLayout->addWidget(cbDisplay);
                headerLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum));
                btnDeleteObject = new QPushButton(this);
                btnDeleteObject->setObjectName("btnDeleteObject");
                btnDeleteObject->setFixedSize(16,16);
                btnDeleteObject->setStyleSheet(QString::fromUtf8("QPushButton#btnDeleteObject\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/delete.png);\n"
                    "    margin: 0;\n"
                    "    padding: 0;\n"
                    "    border: none;\n"
                    "}\n"
                    "\n"
                    "QPushButton#btnDeleteObject:hover\n"
                    "{	\n"
                    "	background-image: url(:/icons/ressources/delete_light.png);\n"
                    "}"));
                btnDeleteObject->setToolTip(QString::fromUtf8("Delete"));
                headerLayout->addWidget(btnDeleteObject);
        mainLayout->addLayout(headerLayout);

        QMetaObject::connectSlotsByName(this);
    }

    bool Layer::SetParameter(int /* parameterId*/, int /*value*/)
    {
        return false;
    }

    bool Layer::SetParameter(int /* parameterId*/, bool /*value*/)
    {
        return false;
    }

    bool Layer::SetParameter(int /* parameterId*/, const QString& /*value*/)
    {
        return false;
    }

    void Layer::setupSpecificInterface()
    {
        return;
    }
} // namspace CCF