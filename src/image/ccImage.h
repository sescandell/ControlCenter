#ifndef CC_IMAGE_HEADER
#define CC_IMAGE_HEADER

/*---------------------------------------------------------- Extern includes */
#include <highgui.h>
#include <QMutex>


/*---------------------------------------------------------- Intern includes */

namespace CCF
{
    class Image: public QMutex
	{
		public:
            /// <summary>
            ///     Constructor
            /// </summary>
            /// <param name="_image">Defines an external IplImage to use in this class</param>
            Image( IplImage* _image = 0 );

            /// <summary>
            ///     Destructor
            /// </summary>
            virtual ~Image();

            /// <summary>
            ///     = operator
            /// </summary>
            void operator=(IplImage* _image);

            /// <summary>
            ///     cast operator to <IplImage *>
            /// </summary>
            operator IplImage *() const;
            
            /// <summary>
            ///     Retrieve the internal IplImage*
            /// </summary>
            IplImage* GetIplImage() const;

            /// <summary>
            ///     Defines the internal IplImage* to <_image>
            /// </summary>
            /// <param name="_image">The external IplImage* to use</param>
            void SetIplImage(IplImage* _image);

            /// <summary>
            ///     Allocate memory for <image>
            /// </summary>
            /// <param name="_width">Width desired</param>
            /// <param name="_height">Height desired</param>
            /// <param name="_channels">Channels count desired</param>
            /// <param name="_depth">Depth desired</param>
            void CreateIplImage(int _width, int _height, int _channels, int _depth);

            /// <summary>
            ///     Free memory allocation used for <image>
            /// </summary>
            /// <warning>Use only if we are the creators of the IplImage*</warning>
            void ReleaseIplImage();

            /// <summary>
            ///     Returns the width of <image>
            /// </summary>
            int GetWidth() const;

            /// <summary>
            ///     Returns the height of <image>
            /// </summary>
            int GetHeight() const;

            /// <summary>
            ///     Returns the size of <image>
            /// </summary>
            CvSize GetSize() const;

            /// <summary>
            ///     Returns the channels count of <image>
            /// </summary>
            int GetChannelsCount() const;

            /// 
            void SetUndistortionParameters(CvMat* intrinsicMatrix, CvMat* distortionCoeffs);

            void Undistort();

			void ResetUndistortParameters();

            void SetRoi(CvRect& rect);
            CvRect GetRoi() const;
			void EqualizeHistogram();
            void NormalizeHistogram();


		protected:
            /*--------------------------------------------- Protected methods*/
            void releaseUndistortionMatrices();

            /*------------------------------------------ Protected attributes*/
            IplImage* image;                // Encapsulated IplImage*
            bool isCreator;                 // Do the class have allocated memory by itself for IplImage ?
            CvRect    roi;
            IplImage* undistortMapX;
            IplImage* undistortMapY;

		private:
            /*----------------------------------------------- Private methods*/

            /*-------------------------------------------- Private attributes*/


	}; // class Image
} // namespace CCF

#endif // #ifndef CC_IMAGE_HEADER