#ifndef CC_VS_FILE_HEADER
#define CC_VS_FILE_HEADER

/*---------------------------------------------------------- Extern includes */
#include <highgui.h>                  

/*---------------------------------------------------------- Intern includes */
#include "ccVideoSource.h"

namespace CCF
{
    class VSFile : public VideoSource
	{
		public:
            /// <summary>
            ///     Constructor
            /// </summary>
            VSFile();

            /// <summary>
            ///     Destructor : close the stream
            /// </summary>
            ~VSFile();

            /// <summary>
            ///     Open the stream corresponding on the default device
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

		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/

	}; // class VSFile
} // namespace CCF

#endif // #ifndef CC_VS_FILE_HEADER