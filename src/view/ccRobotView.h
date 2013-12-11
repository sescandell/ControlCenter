#ifndef CC_ROBOT_VIEW_HEADER
#define CC_ROBOT_VIEW_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QPainter>


/*---------------------------------------------------------- Intern includes */
#include "ccView.h"
#include "../app/ccDisplayable.h"
#include "../robot/ccRobot.h"

namespace CCF
{
    class RobotView: public View
	{
		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            RobotView(const Displayable* _object);

            virtual void Draw(QPainter& painter);

            static void SetRobotSize(int _size);

        protected:
            /*--------------------------------------------- Protected methods*/
            const Robot* robot() const;

            /*------------------------------------------ Protected attributes*/   
            
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class RobotView
} // namespace CCF

#endif // #ifndef CC_ROBOT_VIEW_HEADER