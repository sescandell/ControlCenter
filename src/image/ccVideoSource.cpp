#include "ccVideoSource.h"

#define CC_VIDEOSOURCE_RED_OFFSET       2
#define CC_VIDEOSOURCE_GREEN_OFFSET     1
#define CC_VIDEOSOURCE_BLUE_OFFSET      0


namespace CCF
{
    /* ------------------------------------------ Constructors / Destructors */
    VideoSource::VideoSource():realTime(true),width(0),height(0)
    {
        informations = "Unknown";
    }



    VideoSource::~VideoSource()
    {
        // Nothing to do
    }





    /* ------------------------------------------------------ Public methods */
    QString VideoSource::GetInformations()
    {
        return informations;
    }


    int VideoSource::GetRedOffset()
    {
        return CC_VIDEOSOURCE_RED_OFFSET;
    }


    int VideoSource::GetGreenOffset()
    {
        return CC_VIDEOSOURCE_GREEN_OFFSET;
    }


    int VideoSource::GetBlueOffset()
    {
        return CC_VIDEOSOURCE_BLUE_OFFSET;
    }


    int VideoSource::GetWidth() const
    {
        return width;
    }



    int VideoSource::GetHeight() const
    {
        return height;
    }
    /* --------------------------------------------------- Protected methods */


    /* ----------------------------------------------------- Private methods */
} // namespace CCF