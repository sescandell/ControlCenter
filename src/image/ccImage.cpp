#include "ccImage.h"

#include <cv.h>

namespace CCF
{
    /*-------------------------------------------- Constructors / Destructos */
    Image::Image(IplImage* _image)
        :QMutex(), image(_image), isCreator(false), undistortMapX(0),undistortMapY(0)
    {
    }



    Image::~Image()
    {
        if(isCreator)
            ReleaseIplImage();
        releaseUndistortionMatrices();
        image = 0;
    }




    /*------------------------------------------------------------ Operators */
    void Image::operator =(IplImage* _image)
    {
        SetIplImage(_image);
    }


    
    Image::operator IplImage *() const
    {
        return image;
    }

    
    
    


    
    /* ------------------------------------------------------ Public methods */
    IplImage* Image::GetIplImage() const
    {
        return image;
    }



    void Image::SetIplImage(IplImage *_image)
    {
        if(isCreator) ReleaseIplImage();
        releaseUndistortionMatrices();
        image = _image;
        SetRoi(cvRect(0,0,image->width,image->height));
    }

    void Image::SetRoi(CvRect &rect)
    {
        roi = rect;
    }

    CvRect Image::GetRoi() const
    {
        return roi;
    }



    void Image::CreateIplImage(int _width, int _height, int _channels, int _depth)
    {
        SetIplImage(cvCreateImage(cvSize(_width,_height),_depth,_channels));
        isCreator   = true;
    }

    void Image::ReleaseIplImage()
    {
        if(image) cvReleaseImage(&image); image = 0;

        isCreator = false;        
    }


    int Image::GetWidth() const
    {
        return (image?image->width:0);
    }

    int Image::GetHeight() const
    {
        return (image?image->height:0);
    }

    CvSize Image::GetSize() const
    {
        return cvSize(image?image->width:0,image?image->height:0);
    }

    int Image::GetChannelsCount() const
    {
        return (image?image->nChannels:0);
    }

    void Image::SetUndistortionParameters(CvMat* intrinsicMatrix, CvMat* distortionCoeffs)
    {
        if(!image) return;

        if(!undistortMapX) undistortMapX = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
        if(!undistortMapY) undistortMapY = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );

        cvInitUndistortMap( intrinsicMatrix, distortionCoeffs, undistortMapX, undistortMapY);
    }

    void Image::Undistort()
    {
        if(!image || !undistortMapX || !undistortMapY) return;

        IplImage *t = cvCloneImage( image );
        cvRemap( t, image, undistortMapX, undistortMapY ); // undistort image
		cvReleaseImage( &t );
    }

	void Image::ResetUndistortParameters()
	{
		releaseUndistortionMatrices();
	}

	void Image::EqualizeHistogram()
	{
		cvEqualizeHist(image,image);
	}

    void Image::NormalizeHistogram()
    {
        if(image->nChannels!=1)
            return;
            
        char _min         = 255;
        char _max         = 0;
        int _widthStep    = image->widthStep;
        int _width        = image->width;
        int _height       = image->height;
        char * _imageData = image->imageData;
        int _channels     = image->nChannels;
        for(int y=0; y<_height; y++)
        {
            for(int x=0;x<_width;x++)
            {
               if(_imageData[_channels*(y*_widthStep+x)]<_min) _min = _imageData[_channels*(y*_widthStep+x)];
               if(_imageData[_channels*(y*_widthStep+x)]>_max) _max = _imageData[_channels*(y*_widthStep+x)];
            }
        }
        
        //_min est maintenant le nvg minimum et _max le nvg maximum
        char diff = _max - _min;
        
        //On parcourt l'image une seconde fois
        for(int y=0; y<_height; y++)
        {
            for(int x=0;x<_width;x++)
               _imageData[_channels*(y*_widthStep+x)] = 255*(_imageData[_channels*(y*_widthStep+x)]-_min)/diff;
        }
        
        //Suppression du bruit
        cvSmooth(image,image,CV_MEDIAN,5);

       
    }


    /* --------------------------------------------------- Protected methods */

    void Image::releaseUndistortionMatrices()
    {
        if(undistortMapX)    cvReleaseImage(&undistortMapX);  undistortMapX    = 0;
        if(undistortMapY)    cvReleaseImage(&undistortMapY);  undistortMapY    = 0;
    }
    /* ----------------------------------------------------- Private methods */
} // namespace CCF