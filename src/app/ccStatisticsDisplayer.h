#ifndef CC_STATISTCS_DISPLAYER_HEADER
#define CC_STATISTCS_DISPLAYER_HEADER

#include <QtGui/QWidget>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtCore/QString>

#include "../robot/ccRobot.h"


namespace CCF
{
    class StatisticsDisplayer: public QWidget
    {
        Q_OBJECT

        public:
            StatisticsDisplayer(const Robot* _robot, QWidget* parent=0);

            int GetIdentifier() const;

        public slots:
            inline void SetDisplayRSSI(bool visible)
                { displayRSSI = visible; }
            inline void SetDisplayLQI(bool visible)
                { displayLQI = visible; }
            inline void SetDisplayDelay(bool visible)
                { displayDelay = visible; }

        signals:

        protected:
            QString formatPortName(const QString& name) const;
            void paintEvent(QPaintEvent* ev);
            void drawCurves(QPainter& p) const;
            void drawGrid(QPainter& p) const;

            const Robot* robot;

            bool displayRSSI;
            bool displayLQI;
            bool displayDelay;

        protected slots:

        private:

    }; // class StatisticsDisplayer
} // namespace CCF
#endif // #ifndef CC_STATISTCS_DISPLAYER_HEADER