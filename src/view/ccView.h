#ifndef CC_VIEW_HEADER
#define CC_VIEW_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QPainter>
#include <Qpoint>
#include <QBrush>
#include <QPen>




/*---------------------------------------------------------- Intern includes */
#include "ccControlCenterView.h"
#include "../app/ccDisplayable.h"

namespace CCF
{
    class ControlCenterView;

    class View
	{
		public:
            /// <summary>
            ///     Destructor
            ///</summary>
            virtual ~View();

            static void DefineControlCenterView(const ControlCenterView* _ControlCenterView);
            virtual void Draw(QPainter& painter);

            void SetBrush(const QBrush& _brush);
            void SetPen(const QPen& _pen);

        protected:
            /*--------------------------------------------- Protected methods*/
            /// <summary>
            ///     Constructor
            ///</summary>
            View(const Displayable* _object);

            static QPoint convertCoordinateToDisplayerSize(const QPoint& point);
            static int convertDistanceToDisplayerDistance(int distance);

            /*------------------------------------------ Protected attributes*/     
            //static const double CC_VIEW_PI = 3.14159265;
            QBrush brush;
            const Displayable* object;
            QPen   pen;
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/
            static const ControlCenterView* controlCenterView;

	}; // class View
} // namespace CCF

#endif // #ifndef CC_VIEW_HEADER