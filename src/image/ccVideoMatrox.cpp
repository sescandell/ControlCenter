/*---------------------------------------------------------- Extern includes */


/*---------------------------------------------------------- Intern includes */
#include "ccVideoMatrox.h"

#define CC_BUF_ALLOC_COLOR_ATTR_FLAGS   M_IMAGE+M_GRAB+M_BGR24+M_PACKED
#define CC_BUF_ALLOC_2D_ATTR_FLAGS      M_IMAGE+M_GRAB

namespace CCF
{
    /*-------------------------------------------- Constructors / Destructors */
    VideoMatrox::VideoMatrox():VideoSource(),
        milApplication(0),milSystem(0),milDigitizer(0),milBuffer(0),
        depth(0),channels(0)
    {}



    VideoMatrox::~VideoMatrox()
    {
        Close();
    }




    /*------------------------------------------------------------ Operators */







    /* ------------------------------------------------------ Public methods */
    void VideoMatrox::Open()
    {
        MappAlloc(M_DEFAULT, &milApplication);
        MsysAlloc(M_SYSTEM_GIGE_VISION, M_DEFAULT, M_DEFAULT, &milSystem);
        MdigAlloc(milSystem, M_DEFAULT, MT("M_DEFAULT"), M_DEFAULT, &milDigitizer);
	    
        MIL_INT _width;
        MIL_INT _height;
	    MdigInquire(milDigitizer,M_SIZE_X,&_width);
	    MdigInquire(milDigitizer,M_SIZE_Y,&_height);
        MdigInquire(milDigitizer,M_SIZE_BIT,&depth);
        MdigInquire(milDigitizer,M_SIZE_BAND,&channels);
        height=_height;
        width=_width;

        //MdigControl(milDigitizer, M_CORRUPTED_FRAME_ERROR, M_DISABLE);


	    //MbufAllocColor(milSystem, channels, width, height, depth, CC_BUF_ALLOC_COLOR_ATTR_FLAGS, &milBuffer);
        MbufAlloc2d(milSystem, width, height, depth, CC_BUF_ALLOC_2D_ATTR_FLAGS, &milBuffer);
	    MbufClear(milBuffer, 0);

        MdigGrabContinuous(milDigitizer,milBuffer);
    }

    void VideoMatrox::Close()
    {
        MdigHalt(milDigitizer);
        MbufFree(milBuffer);
	    MdigFree(milDigitizer);
	    MsysFree(milSystem);
	    MappFree(milApplication);
    }

    void VideoMatrox::GetFrame(Image& imageDest)
    {
        //MdigGrab(milDigitizer,milBuffer);
        if(!imageDest.GetIplImage())
        {
            //imageDest.CreateIplImage(1280,1024,channels,depth);
            imageDest.CreateIplImage(width,height,channels,depth);
        }
        //MbufGet2d(milBuffer,584,512,1280,1024, imageDest.GetIplImage()->imageData);
        MbufGet2d(milBuffer,0,0,width,height,imageDest.GetIplImage()->imageData);
    }



    /* --------------------------------------------------- Protected methods */





    /* ----------------------------------------------------- Private methods */
} // namespace CCF