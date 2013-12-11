#ifndef CC_VIDEO_MATROX_HEADER
#define CC_VIDEO_MATROX_HEADER

/*---------------------------------------------------------- Extern includes */
#include <cv.h>
#include <highgui.h>
#include "mil.h"


/*---------------------------------------------------------- Intern includes */
#include "ccVideoSource.h"

namespace CCF
{
    class VideoMatrox : public VideoSource
	{
		public:            
            /// <summary>
            ///     Constructor
            /// </summary>
            VideoMatrox();

            /// <summary>
            ///     Destructor : close the stream
            /// </summary>
            ~VideoMatrox();

            /// <summary>
            ///     Open the stream
            /// </summary>
            virtual void Open();

            /// <summary>
            ///     Close the stream
            /// </summary>
            virtual void Close();

            /// <summary>
            ///     Return a frame from the video source
            /// </summary>
            /// <param name="imageDest">The image destination to store the frame</param>
            virtual void GetFrame(Image& imageDest);

		protected:
            /*--------------------------------------------- Protected methods*/

            /*------------------------------------------ Protected attributes*/
            MIL_ID milApplication;
            MIL_ID milSystem;
            MIL_ID milDigitizer;
            MIL_ID milBuffer;
            
            MIL_INT depth;
            MIL_INT channels;


		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class VideoMatrox
} // namespace CCF

#endif // #ifndef CC_VIDEO_MATROX_HEADER