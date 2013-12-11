#include "ccImageProcessing.h"

#include <cv.h>

#include <QtCore/QTime>

#include "ccVSFile.h"
#include "ccWebcam.h"
#include "ccVideoMatrox.h"
#include "ccIPCUndistortion.h"
#include "ccIPKhepera.h"

#define VIDEO_OBJECT_NAME               VideoMatrox()

namespace CCF
{
    static const int CC_IP_INTERPOLATION_RESIZE                             = CV_INTER_NN;

    static const char CC_IPC_UNDISTORTION_KEY[]                             = "ipcUndistortion";
    static const char CC_IP_KHEPERA_KEY[]                                   = "ipKhepera";

    static const int CC_IMAGE_PROCESSING_GET_SYNCHRO_CU_RESSOURCES          = 1;
    static const int CC_IMAGE_PROCESSING_WAIT_TIME                          = 20;

    static const int CC_IMAGE_PROCESSING_SYNCHRO_GET_COUNT_RESSOURCES       = 1;
    static const int CC_IMAGE_PROCESSING_SYNCHRO_PUT_COUNT_RESSOURCES       = 1;

    static const int CC_IMAGE_PROCESSING_FRAME_COUNT_FPS                    = 20;


    ImageProcessing::ImageProcessing(QSemaphore* _semSynchroControlUnit, QObject * parent)
        :QThread(parent), semSynchroControlUnit(_semSynchroControlUnit)
    {
        displayImage         = 0;
        displayActivated     = true;
        calibrationActivated = true;
        normalizationActivated = false;
        imageProcessor       = 0;
        grabFrame            = true;
        stop                 = false;
        waitControlUnit      = false;

        start();
        synchro.acquire(CC_IMAGE_PROCESSING_SYNCHRO_GET_COUNT_RESSOURCES);
    }

    ImageProcessing::~ImageProcessing()
    {}

    void ImageProcessing::DefineRoi(CvRect& rect)
    {
        // TODO : ajouter taille image originale
        float scaleX = (float)(image.GetWidth())/(float)(displayImage->GetWidth());
        float scaleY = (float)(image.GetHeight())/(float)(displayImage->GetHeight());
        
        rect.x = rect.x*scaleX;
        rect.y = rect.y*scaleY;
        rect.width = rect.width*scaleX;
        rect.height = rect.height*scaleY;

        mutexGrabFrame.lock();
        image.SetRoi(rect);
        mutexGrabFrame.unlock();
    }

    void ImageProcessing::InitializeUndistortion(int boardWidth, int boardHeight, int boardCount)
    {     
        mutexGrabFrame.lock();
        ((IPCUndistortion*)(imageProcessors[CC_IPC_UNDISTORTION_KEY]))->SetParameters(boardWidth, boardHeight, boardCount);
	    image.ResetUndistortParameters();
        mutexGrabFrame.unlock();
    }

	QSize& ImageProcessing::GetVideoSourceSize() const
	{
		return QSize(videoSource->GetWidth(),videoSource->GetHeight());
	}

    void ImageProcessing::SetDisplayActivated(bool value)
    {
        // On pourrait eventuellement le proteger ...
        // mais c'est pas réellement nécessaire
        displayActivated = value;
    }

    void ImageProcessing::SetCalibrationActivated(bool value)
    {
        // On pourrait eventuellement le proteger ...
        // mais c'est pas réellement nécessaire si on considère
        // que l'écriture est atomique
        calibrationActivated = value;
    }

    void ImageProcessing::SetNormalizationActivated(bool value)
    {
        // On pourrait eventuellement le proteger ...
        // mais c'est pas réellement nécessaire si on considère
        // que l'écriture est atomique
        normalizationActivated = value;
    }

    int ImageProcessing::GetFPS()
    {
        mutexGrabFrame.lock();
        int value = fps;
        mutexGrabFrame.unlock();

        return value;
    }

    void ImageProcessing::SetKheperaSize(int kSize)
    {
        mutexGrabFrame.lock();
        if(imageProcessors[CC_IP_KHEPERA_KEY])
            ((IPKhepera*)imageProcessors[CC_IP_KHEPERA_KEY])->SetKheperaSize(kSize);
        mutexGrabFrame.unlock();
    }


    void ImageProcessing::Play()
    {
        // Protect this variable by mutex permits us to
        // be sure that when we call Play() processing will start
        mutexGrabFrame.lock();
        grabFrame = true;
        mutexGrabFrame.unlock();
    }

    void ImageProcessing::Pause()
    {
        // Protect this variable by mutex permits us to
        // be sure that when we call Pause() processing is stopped
        mutexGrabFrame.lock();
        grabFrame = false;
        mutexGrabFrame.unlock();
    }

    void ImageProcessing::Stop()
    {
        mutexStop.lock();
        stop = true;
        mutexStop.unlock();
    }

    void ImageProcessing::SetDisplayableImage(Image* _displayImage)
    {
        // We wait for the processing to be terminated
        mutexGrabFrame.lock();
        displayImage = _displayImage;
        mutexGrabFrame.unlock();
    }

    bool ImageProcessing::SetImageProcessing(ImgProcessors value)
    {
        if(!isRunning()) return false;
        
        // We wait for the processing terminated
        mutexGrabFrame.lock();
        switch(value)
        {                
            case IP_KHEPERA: 
                imageProcessor  = imageProcessors[CC_IP_KHEPERA_KEY];
                waitControlUnit = true;
                break;
            case IPC_UNDISTORTION: 
                imageProcessor  = imageProcessors[CC_IPC_UNDISTORTION_KEY];
                waitControlUnit = false;
                break;
            case IP_NONE:
            default: 
                imageProcessor = 0;
                break;
        }
        mutexGrabFrame.unlock();

        return imageProcessor!=0 || value==IP_NONE;
    }


    void ImageProcessing::run()
    {
        /////////////////// Initialization
        // Video Source creation
        videoSource = new VIDEO_OBJECT_NAME;
        videoSource->Open();

        // Undistortion Image Processor
        imageProcessors.insert(CC_IPC_UNDISTORTION_KEY, new IPCUndistortion());
        connect(((IPCalibration*)imageProcessors[CC_IPC_UNDISTORTION_KEY]),   SIGNAL(ProcessTerminated()), this, SLOT(endOfUndistortionCalibration()));
        connect(((IPCUndistortion*)imageProcessors[CC_IPC_UNDISTORTION_KEY]), SIGNAL(BoardMatched()),      this, SIGNAL(BoardMatched()), Qt::DirectConnection);
        
        // Khepera finder Image Processor
        imageProcessors.insert(CC_IP_KHEPERA_KEY, new IPKhepera());
        connect(((IPKhepera*)imageProcessors[CC_IP_KHEPERA_KEY]), SIGNAL(CirclesFound(IPKListPositions)), this, SIGNAL(NewPositionsFound(IPKListPositions)), Qt::DirectConnection);

        // Signal that we are ready
        synchro.release(CC_IMAGE_PROCESSING_SYNCHRO_PUT_COUNT_RESSOURCES);





        ////////////////// Running
        lastFpsTimestamp = generateTimeStamp();
        int frameCount = 0;
        mutexStop.lock();
        while(!stop)
        {
            mutexStop.unlock();            
            if(waitControlUnit)
                semSynchroControlUnit->acquire(CC_IMAGE_PROCESSING_GET_SYNCHRO_CU_RESSOURCES);

            mutexGrabFrame.lock();
            if(grabFrame)
            {
                videoSource->GetFrame(image);

                if(image.GetIplImage())
                {
                    if(++frameCount==CC_IMAGE_PROCESSING_FRAME_COUNT_FPS)
                    {
                        int currentTime = generateTimeStamp();
                        fps = CC_IMAGE_PROCESSING_FRAME_COUNT_FPS*1000/(currentTime-lastFpsTimestamp+1);    // +1 just to easily avoid a division by zero
                        lastFpsTimestamp = currentTime;                                                     // without creating a big error in the result
                        frameCount = 0;
                    }

                    //if(normalizationActivated) image.EqualizeHistogram();
                    if(normalizationActivated) image.NormalizeHistogram();
                    if(calibrationActivated) image.Undistort();			                                        

                    if(imageProcessor) imageProcessor->ProcessImage(image);                

                    if(displayActivated) copyImageToDisplayImage();   

                }

                emit ImageProcessed();
            }
            else
            {
                if(waitControlUnit)
                    semSynchroControlUnit->release(CC_IMAGE_PROCESSING_GET_SYNCHRO_CU_RESSOURCES);
            }
            mutexGrabFrame.unlock();
            //msleep(CC_IMAGE_PROCESSING_WAIT_TIME);
            mutexStop.lock();            
        }
        mutexStop.unlock();






        /////////////////// Finalization
        delete videoSource;
        delete imageProcessors[CC_IPC_UNDISTORTION_KEY];  // We can do better using iterators or something else
        delete imageProcessors[CC_IP_KHEPERA_KEY];        // to destroy all objects
    }

    int ImageProcessing::generateTimeStamp()
    {
        QTime t = QTime::currentTime();        
        return t.msec() + 1000*(t.second() + 60 * (t.minute() + 60 * t.hour()));
    }



    void ImageProcessing::copyImageToDisplayImage()
    {
        if(displayImage && displayImage->tryLock())
        {
            if(image.GetWidth()!=displayImage->GetWidth() || image.GetHeight()!=displayImage->GetHeight())
                cvResize(image.GetIplImage(),displayImage->GetIplImage(),CC_IP_INTERPOLATION_RESIZE);
            else
                cvCopy(image.GetIplImage(),displayImage->GetIplImage());
            
            displayImage->unlock();
        }
    }

    void ImageProcessing::endOfCalibration()
    {     
		imageProcessor = 0;
        emit CalibrationEnded();
    }

    void ImageProcessing::endOfUndistortionCalibration()
    {
        image.SetUndistortionParameters( ((IPCUndistortion*)(imageProcessor))->GetIntrinsicMatrix(), ((IPCUndistortion*)(imageProcessor))->GetDistortionCoefficients());
        endOfCalibration();
    }

    bool ImageProcessing::SaveUndistortionParametersToFile(const QString& file)
    {
        if(!isRunning()) return false;

        return ((IPCalibration*)(imageProcessors[CC_IPC_UNDISTORTION_KEY]))->SaveToFile(file);
    }

    bool ImageProcessing::LoadUndistortionParametersFromFile(const QString& file)
    {
        if(!isRunning()) return false;
        
        mutexGrabFrame.lock();
        bool loaded = ((IPCalibration*)(imageProcessors[CC_IPC_UNDISTORTION_KEY]))->LoadFromFile(file);
        image.SetUndistortionParameters( ((IPCUndistortion*)(imageProcessors[CC_IPC_UNDISTORTION_KEY]))->GetIntrinsicMatrix(), ((IPCUndistortion*)(imageProcessors[CC_IPC_UNDISTORTION_KEY]))->GetDistortionCoefficients());
        mutexGrabFrame.unlock();

        return loaded;
    }
} // namespace CCF