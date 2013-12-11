#ifndef CC_IPCUNDISTORTION_HEADER
#define CC_IPCUNDISTORTION_HEADER

#include <QString>
#include <cv.h>

#include "ccIPCalibration.h"
#include "ccImage.h"

namespace CCF
{
    class IPCUndistortion: public IPCalibration
    {
        Q_OBJECT

        public:
            IPCUndistortion();
            ~IPCUndistortion();
            
            CvMat* GetDistortionCoefficients() const;
            CvMat* GetIntrinsicMatrix() const;
            bool LoadFromFile(const QString& file);
            void ProcessImage(Image& image);
            void Reset();
            bool SaveToFile(const QString& file);
            void SetParameters(int _boardWidth, int _boardHeight, int _boardCount);

        public slots:
            
        signals:
            void BoardMatched();

        protected:
            void addCorners();
            bool findCorners(Image& image);
            void generateMatrices(Image& image);
            void initialize();

            int     boardHeight;                // In squares
            int     boardWidth;                 // In squares
            int     boardCount;                 // Count of board to succeed to generate matrices
            CvPoint2D32f*   corners;
            CvMat*  distortionCoeffs;
            int     frameCount;                 // Count of frame used to try analyze
            CvMat*  imagePoints;
            CvMat*  intrinsicMatrix;
            CvMat*  objectPoints;
            CvMat*  pointsCount;
            int     successCount;               // Count of frame analyzed succesfully


        private:

    }; // class IPCUndistortion
} // namespace CCF



#endif // #ifndef CC_IPCUNDISTORTION_HEADER