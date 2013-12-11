#include "ccVSFile.h"

#include <cv.h>

namespace CCF
{
    /*-------------------------------------------- Constructors / Destructors */
    VSFile::VSFile():VideoSource()
    {
        width = 2448;
        height = 2050;
    }



    VSFile::~VSFile()
    {
    }








    /*------------------------------------------------------------ Operators */







    /* ------------------------------------------------------ Public methods */
    void VSFile::Open()
    {
    }


    void VSFile::Close()
    {
    }



    void VSFile::GetFrame(Image& imageDest)
    {
        static bool done = false;
        static IplImage* lastImage = 0;
        static IplImage* image = cvLoadImage("Debug/imageClaire.bmp",0);
        //if(done) return;
        done = true;
        cvReleaseImage(&lastImage);
        lastImage = cvCloneImage(image);
        //cvAddS(lastImage,cvScalar(70),lastImage);
            //IplImage* frame = cvQueryFrame(source);
        if(!imageDest.GetIplImage())
            imageDest.CreateIplImage(width,height,1,IPL_DEPTH_8U);
        imageDest.SetIplImage(lastImage);
        //if(frame) cvCvtColor(frame,imageDest,CV_BGR2GRAY);
    }


    /* --------------------------------------------------- Protected methods */


    /* ----------------------------------------------------- Private methods */
} // namespace CCF