#include "ccWebcam.h"

#include <cv.h>

namespace CCF
{
    /*-------------------------------------------- Constructors / Destructors */
    Webcam::Webcam(int _sourceIndex):VideoSource(), sourceIndex(_sourceIndex), source(NULL)
    {}



    Webcam::~Webcam()
    {
        Close();
    }








    /*------------------------------------------------------------ Operators */







    /* ------------------------------------------------------ Public methods */
    void Webcam::Open()
    {
        Open(sourceIndex);
    }



    void Webcam::Open(int index)
    {
        sourceIndex = index;
        source = cvCaptureFromCAM(sourceIndex);
        cvQueryFrame(source);       // This call is necessary to get
                                    // correct capture properties
        width  = cvGetCaptureProperty(source,CV_CAP_PROP_FRAME_WIDTH);
        height = cvGetCaptureProperty(source,CV_CAP_PROP_FRAME_HEIGHT);
    }



    void Webcam::Close()
    {
        if(source)
            cvReleaseCapture(&source);
        
        source = NULL;
    }



    void Webcam::GetFrame(Image& imageDest)
    {
        IplImage* frame = cvQueryFrame(source);
        if(!frame) return;
        if(!imageDest.GetIplImage())
            imageDest.CreateIplImage(frame->width,frame->height,1,IPL_DEPTH_8U);
        cvCvtColor(frame,imageDest,CV_BGR2GRAY);
    }


    /* --------------------------------------------------- Protected methods */


    /* ----------------------------------------------------- Private methods */
} // namespace CCF