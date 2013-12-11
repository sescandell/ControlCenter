#ifndef CC_TRACK_VIEW_HEADER
#define CC_TRACK_VIEW_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QPainter>


/*---------------------------------------------------------- Intern includes */
#include "ccView.h"
#include "../app/ccDisplayable.h"
#include "../track/ccTrack.h"

namespace CCF
{
    class TrackView: public View
	{
		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            TrackView(const Displayable* _object, bool _drawFlags = true);


            virtual void Draw(QPainter& painter);

        protected:
            /*--------------------------------------------- Protected methods*/
            bool         drawFlags;
            const Track* track() const;

            /*------------------------------------------ Protected attributes*/   
            
            
    
        private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class TrackView
} // namespace CCF

#endif // #ifndef CC_TRACK_VIEW_HEADER