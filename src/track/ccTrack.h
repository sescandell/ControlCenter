#ifndef CC_TRACK_HEADER
#define CC_TRACK_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QMutex>
#include <QPolygon>
#include <QString>

#include "../app/ccDisplayable.h"


/*---------------------------------------------------------- Intern includes */


namespace CCF
{
    class Track:public Displayable, public QPolygon, public QMutex
	{
		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            Track(const QString & _name);

        protected:
            /*--------------------------------------------- Protected methods*/
            

            /*------------------------------------------ Protected attributes*/

            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class Track
} // namespace CCF

#endif // #ifndef CC_TRACK_HEADER