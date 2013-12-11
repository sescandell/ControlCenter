#ifndef CC_CONTROL_CENTER_DISPLAYER_HEADER
#define CC_CONTROL_CENTER_DISPLAYER_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

/*---------------------------------------------------------- Intern includes */


namespace CCF
{
    class ControlCenterDisplayer: public QWidget
	{
        Q_OBJECT

		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            ControlCenterDisplayer(QWidget *parent = 0);

            void DisableTool();
            void EnableTool();

        public slots:

        signals:
            void clicked(QPoint);
            void doubleClicked(QPoint);
            void mouseMoved(QPoint);
            void painting(QPainter&); // Must be connected with a DirectConnection
            void resized(QSize);

        protected:
            /*--------------------------------------------- Protected methods*/
            void paintEvent(QPaintEvent *ev);
            void mouseDoubleClickEvent(QMouseEvent *ev);
            void mouseMoveEvent(QMouseEvent * ev);
            void mousePressEvent(QMouseEvent* ev);            
            void resizeEvent(QResizeEvent* ev);

            /*------------------------------------------ Protected attributes*/
            bool tool;

            
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class ControlCenterDisplayer
} // namespace CCF

#endif // #ifndef CC_CONTROL_CENTER_DISPLAYER_HEADER