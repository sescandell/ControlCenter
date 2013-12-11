#ifndef CC_WEBCAM_HEADER
#define CC_WEBCAM_HEADER

/*---------------------------------------------------------- Extern includes */
#include <highgui.h>                  

/*---------------------------------------------------------- Intern includes */
#include "ccVideoSource.h"

namespace CCF
{
    class Webcam : public VideoSource
	{
		public:
            /// <summary>
            ///     Constructor
            /// </summary>
            Webcam(int _sourceIndex = 0);

            /// <summary>
            ///     Destructor : close the stream
            /// </summary>
            ~Webcam();

            /// <summary>
            ///     Open the stream corresponding on the default device
            /// </summary>
            virtual void Open();

            /// <summary>
            ///     Open the stream corresponding on the device <index>
            /// </summary>
            /// <param name="index">Device identifier</param>
            void Open(int index);

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
            int sourceIndex;                          // Camera index
            CvCapture* source;                        // A pointer on the camera

		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class Webcam
} // namespace CCF

#endif // #ifndef CC_WEBCAM_HEADER