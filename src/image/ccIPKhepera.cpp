#include "ccIPKhepera.h"

#include <cv.h>

namespace CCF
{
    // Hough parameters
    static const int CC_IP_KHEPERA_HOUGH_DP                     = 2;
    static const int CC_IP_KHEPERA_HOUGH_METHOD                 = CV_HOUGH_GRADIENT;
    static const int CC_IP_KHEPERA_HOUGH_CANNY_THRESHOLD        = 200;
    static const int CC_IP_KHEPERA_HOUGH_ACCUMULATOR_THRESHOLD  = 50;
    static const double CC_IP_KHEPERA_POURCENT_MIN_RADIUS       = 0.8;
    static const double CC_IP_KHEPERA_POURCENT_MAX_RADIUS       = 1.2;

    // Image resizing constraints
    static const int CC_IP_KHEPERA_IMAGE_MAX_HEIGHT     = 1024;
    static const int CC_IP_KHEPERA_IMAGE_MAX_WIDTH      = 1280;
    static const int CC_IP_KHEPERA_RESIZE_INTERPOLATION = CV_INTER_NN;

    
    
    IPKhepera::IPKhepera()
        :ImageProcessor()
    {

    }

    IPKhepera::~IPKhepera()
    {
        Reset();
    }

    void IPKhepera::Reset()
    {
        // Nothing todo
    }

    void IPKhepera::SetKheperaSize(int _size)
    {
        minRadius = CC_IP_KHEPERA_POURCENT_MIN_RADIUS*_size/2;
        maxRadius = CC_IP_KHEPERA_POURCENT_MAX_RADIUS*_size/2;
    }

    IplImage* IPKhepera::resizeImage(const Image& image) const
    {
        // Compute destination size keeping aspect ratio
        //double ratio = (double)image.GetRoi().width / (double)image.GetRoi().height;
        double ratio = (double)image.GetWidth() / (double)image.GetHeight();
        QSize destinationSize;
        if(ratio>=1) // landscape mode
        {
            destinationSize.setWidth(CC_IP_KHEPERA_IMAGE_MAX_WIDTH);
            destinationSize.setHeight(CC_IP_KHEPERA_IMAGE_MAX_WIDTH/ratio);
            if(destinationSize.height()>CC_IP_KHEPERA_IMAGE_MAX_HEIGHT)
            {
                destinationSize.setHeight(CC_IP_KHEPERA_IMAGE_MAX_HEIGHT);
                destinationSize.setWidth(CC_IP_KHEPERA_IMAGE_MAX_HEIGHT*ratio);
            }
        }
        else // portrait mode
        {
            destinationSize.setHeight(CC_IP_KHEPERA_IMAGE_MAX_HEIGHT);
            destinationSize.setWidth(CC_IP_KHEPERA_IMAGE_MAX_HEIGHT*ratio);
            if(destinationSize.width()>CC_IP_KHEPERA_IMAGE_MAX_WIDTH)
            {
                destinationSize.setWidth(CC_IP_KHEPERA_IMAGE_MAX_WIDTH);
                destinationSize.setHeight(CC_IP_KHEPERA_IMAGE_MAX_WIDTH/ratio);
            }
        }

        // Apply transformation
        IplImage* imageResized = cvCreateImage(cvSize(destinationSize.width(),destinationSize.height()),image.GetIplImage()->depth,image.GetChannelsCount());
        cvResize(image,imageResized,CC_IP_KHEPERA_RESIZE_INTERPOLATION);  // cvResize take into account the ROI
        
        return imageResized;
    }

    void IPKhepera::ProcessImage(Image& image)
    {
        IPKListPositions positions;
        ///////////////////////////
        //    emit CirclesFound(positions);
        //    return;
        ////////////////////////////////
        IplImage* temp;
        float rx = 1.;      // x resize factor
        float ry = 1.;      // y resize factor
        //cvSetImageROI(image,image.GetRoi());
        
        // If the ROI is to big to be processed, we create a temporary image
        //if(image.GetRoi().height>CC_IP_KHEPERA_IMAGE_MAX_HEIGHT || image.GetRoi().width>CC_IP_KHEPERA_IMAGE_MAX_WIDTH)
        if(image.GetHeight()>CC_IP_KHEPERA_IMAGE_MAX_HEIGHT || image.GetWidth()>CC_IP_KHEPERA_IMAGE_MAX_WIDTH)
        {
            temp = resizeImage(image);
            rx = (float)image.GetWidth()  / (float)temp->width;
            ry = (float)image.GetHeight() / (float)temp->height;
        }
        else
        {
            temp = cvCloneImage(image);
        }
        
        

		/*
		IplImage* t = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
        IplConvKernel* kernel = cvCreateStructuringElementEx(CC_IPKHEPERA_KERNEL_SIZE,CC_IPKHEPERA_KERNEL_SIZE,CC_IPKHEPERA_KERNEL_SIZE/2,CC_IPKHEPERA_KERNEL_SIZE/2,CC_IPKHEPERA_KERNEL_SHAPE);
        cvMorphologyEx(image,image,t,kernel,CC_IPKHEPERA_MORPHOLOGY_OPERATION,CC_IPKHEPERA_MORPHOLOGY_ITERATION);

		cvThreshold(image,image,CC_IPKHEPERA_THRESHOLD_VALUE, 255, CC_IPKHEPERA_THRESHOLD_TYPE);
		//IplImage* t2 = cvCloneImage(image);
		//cvAdaptiveThreshold(t2,image,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY_INV);
		//cvSmooth(image,image,CV_MEDIAN,3,3);
        cvReleaseStructuringElement(&kernel);
        //cvReleaseImage(&t2);
		cvReleaseImage(&t);
        //*/        
        
        //*
        //cvCanny(temp,temp,CC_IP_KHEPERA_HOUGH_CANNY_THRESHOLD,CC_IP_KHEPERA_HOUGH_CANNY_THRESHOLD/2);
        //cvSmooth(temp,temp,CV_GAUSSIAN,9,9);

		CvMemStorage* storage = cvCreateMemStorage(0);
        cvSmooth(temp,temp,CV_GAUSSIAN, 9, 9);
        CvSeq* results = cvHoughCircles(
                        temp,
                        storage,
                        CC_IP_KHEPERA_HOUGH_METHOD,
                        CC_IP_KHEPERA_HOUGH_DP,
                        2*minRadius/rx,
						CC_IP_KHEPERA_HOUGH_CANNY_THRESHOLD,
						CC_IP_KHEPERA_HOUGH_ACCUMULATOR_THRESHOLD,
						minRadius/rx,
						maxRadius/rx
                       );
        IPKPosition position;
        
		for( int i = 0; i < results->total; i++ ) 
        {
            float* p = (float*) cvGetSeqElem( results, i );
            //position.setX(cvRound( p[0]*rx+image.GetRoi().x ));			//Coordinates in the full frame
            //position.setY(( p[1]*ry+image.GetRoi().y ));
            position.setX(cvRound( p[0]*rx));			//Coordinates in the full frame
            position.setY(( p[1]*ry));
            positions.append(position);
        }

        //if(positions.size())
            emit CirclesFound(positions);
		cvReleaseMemStorage(&storage);
        //*/        
        cvReleaseImage(&temp);
        //cvResetImageROI(image);
    }
} // namespace CCF