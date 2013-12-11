#ifndef CC_LAYER_HEADER
#define CC_LAYER_HEADER

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

#include <QtCore/QString>

#include "ccDisplayable.h"


namespace CCF
{
    class Layer: public QWidget
    {
        Q_OBJECT

        public:
            enum LayerType { LT_NONE, LT_ROBOT, LT_TRACK };

            Layer(const Displayable* _object, QWidget* parent=0);            
            virtual ~Layer();

            int GetIdentifier() const;
            virtual LayerType GetLayerType() const;
            bool IsSelected() const;
            void SetSelected(bool value);
            virtual bool SetParameter(int parameterId, int value);
            virtual bool SetParameter(int parameterId, bool value);
            virtual bool SetParameter(int parameterId, const QString& value);

        public slots:

        signals:
            void clicked(int /* layer Type */, int /*object identifier*/);
            void DisplayStateChanged(int,bool);
            void DeleteObject(int /* layer Type */, int /*object identifier*/);

        protected:
            void mousePressEvent(QMouseEvent* ev);
            void paintEvent(QPaintEvent* ev);

            const Displayable* object;
            bool selected;

            // Design variables
            void setupInterface();
            virtual void setupSpecificInterface();
            QVBoxLayout* mainLayout;
            QHBoxLayout* headerLayout;
            QLayout*     contentLayout;
            QCheckBox*   cbDisplay;
            QPushButton* btnDeleteObject;

        protected slots:
            void on_cbDisplay_stateChanged(int);
            void on_btnDeleteObject_clicked();

        private:

    }; // class Layer
} // namespace CCF
#endif // #ifndef CC_LAYER_HEADER