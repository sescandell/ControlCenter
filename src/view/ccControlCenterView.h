#ifndef CC_CONTROL_CENTER_VIEW_HEADER
#define CC_CONTROL_CENTER_VIEW_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtGui/QPainter>
#include <QtCore/QSize>
#include <QtGui/QImage>

/*---------------------------------------------------------- Intern includes */
#include "../image/ccImage.h"
#include "../app/ccControlCenter.h"
#include "ccView.h"


namespace CCF
{
    class View;

    class ControlCenterView: public QObject
	{
        Q_OBJECT

		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            ControlCenterView(const ControlCenter* _controlCenter, const QSize& _viewSize, const QSize& _histogramSize);

            /// <summary>
            ///     Destructor
            ///</summary>
            ~ControlCenterView();

            void AddView(int identifier, View* view);
            bool IsPointInImage(const QPoint& point) const;
            QPoint ConvertCoordinateToDisplayerSize(const QPoint& point) const;
            QPoint ConvertCoordinateToVideoSourceSize(const QPoint& point) const;
            int    ConvertDistanceToDisplayerDistance(int distance) const;
            void RemoveView(int identifier);
            static void SetRobotSize(int size);

            Image* GetImage() const;

        public slots:
            void DisplayerResized(QSize newSize);
            void HistogramDisplayerResized(QSize newSize);
            void Draw(QPainter& painter);
            void DrawHistogram(QPainter& painter);
            void SetDisplayImageActivated(int);
            void SetDisplayCirclesActivated(bool);
            void SetDisplayView(int,bool);

        protected:
            /*--------------------------------------------- Protected methods*/

            /*------------------------------------------ Protected attributes*/
            const ControlCenter* controlCenter;
            bool    displayCircles;
            bool    displayImage;
            Image*  image;
            QSize   viewSize;
            QSize   histogramSize;
            QMap<int, View*> views;
            QMap<int, bool>  displayView;
            QImage* qImage;
            QImage* qHistogramImage;
            
            
            
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class ControlCenterView
} // namespace CCF

#endif // #ifndef CC_CONTROL_CENTER_VIEW_HEADER