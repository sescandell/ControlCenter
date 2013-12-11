#ifndef CC_IMAGEPROCESSING_HEADER
#define CC_IMAGEPROCESSING_HEADER

/*----------------------------------------------------------- Extern includes*/
#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include <QMap>
#include <QSize>
#include <highgui.h>

/*----------------------------------------------------------- Intern includes*/
#include "ccImage.h"
#include "ccImageProcessor.h"
#include "ccVideoSource.h"


namespace CCF
{
    enum ImgProcessors { IP_KHEPERA, IPC_UNDISTORTION, IP_NONE };

    class ImageProcessing: public QThread
    {
        Q_OBJECT
        public:
            ImageProcessing(QSemaphore* _semSynchroControlUnit, QObject * parent = 0);
            ~ImageProcessing();

            void DefineRoi(CvRect& rect);
            void InitializeUndistortion(int boardWidth, int boardHeight, int boardCount);
			int GetFPS();
            QSize& GetVideoSourceSize() const;
            bool LoadUndistortionParametersFromFile(const QString& file);
            void Play();
            void Pause();
            bool SaveUndistortionParametersToFile(const QString& file);
            void SetCalibrationActivated(bool value);
            void SetNormalizationActivated(bool value);
            void SetDisplayableImage(Image* _displayImage);
            void SetDisplayActivated(bool value);
            void SetKheperaSize(int kSize);
            bool SetImageProcessing(ImgProcessors value);
            void Stop();

        public slots:            
            // Must not have slot. run() method does not invoke exec() method.
            
        signals:
            void BoardMatched();
            void CalibrationEnded();
            void NewPositionsFound(IPKListPositions);
            void ImageProcessed();
            

        protected:
            void copyImageToDisplayImage();
            void endOfCalibration();
            int generateTimeStamp();
            void run();
            
            bool    calibrationActivated;
            bool    displayActivated;
            Image*  displayImage;
            int     fps;
            bool    grabFrame;
            Image   image;
            ImageProcessor*                 imageProcessor;
            QMap<QString, ImageProcessor*>  imageProcessors;
            int           lastFpsTimestamp;
            QMutex        mutexGrabFrame;
            QMutex        mutexStop;
            bool          normalizationActivated;
            QSemaphore*   semSynchroControlUnit;
            bool          stop;
            QSemaphore    synchro;
            VideoSource*  videoSource;
            bool          waitControlUnit;

        protected slots:
            void endOfUndistortionCalibration();

        private:

    }; // class ImageProcessing
} // namespace CCF

#endif // #ifndef CC_IMAGEPROCESSING_HEADER
