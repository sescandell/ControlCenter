#ifndef CC_VIDEO_SOURCE_HEADER
#define CC_VIDEO_SOURCE_HEADER

/*---------------------------------------------------------- Extern includes */
#include <QString>                  // To contain video source informations

/*---------------------------------------------------------- Intern includes */
#include "ccImage.h"  // To grab a frame we manipulate an Image object

namespace CCF
{
	class VideoSource
	{
		public:
            /// <summary>
            ///     Constructor
            ///</summary>
            VideoSource();

            /// <summary>
            ///     Destructor
            ///</summary>
            virtual ~VideoSource();

            /// <summary>
            ///     Retrieve informations concerning the video source
            /// </summary>
            virtual QString GetInformations();

            /// <summary>
            ///     Open the stream
            /// </summary>
            virtual void Open() = 0;

            /// <summary>
            ///     Close the stream
            /// </summary>
            virtual void Close() = 0;

            /// <summary>
            ///     Return a frame from the video source
            /// </summary>
            /// <param name="imageDest">The image destination to store the frame</param>
            virtual void GetFrame(Image& imageDest) = 0;

            /// <summary>
            ///     Return the red color offset in the image structure
            /// </summary>
            virtual int GetRedOffset();

            /// <summary>
            ///     Return the green color offset in the image structure
            /// </summary>
            virtual int GetGreenOffset();

            /// <summary>
            ///     Return the blue color offset in the image structure
            /// </summary>
            virtual int GetBlueOffset();

            /// <summary>
            ///     Return the image width
            /// </summary>
            virtual int GetWidth() const;

            /// <summary>
            ///     Return the image height
            /// </summary>
            virtual int GetHeight() const;
            
		protected:
            /*--------------------------------------------- Protected methods*/

            /*------------------------------------------ Protected attributes*/
            QString informations;               // Contains some informations about the video source
            bool realTime;                      // Do we need a real time stream ?
            int width;                          // Digitizer image width
            int height;                         // Digitizer image height

		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class VideoSource
} // namespace CCF

#endif // #ifndef CC_VIDEO_SOURCE_HEADER