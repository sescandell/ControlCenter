#include "ccIPCUndistortion.h"
#include <QByteArray>


#define CC_IPCUNDISTORTION_SKIP_FRAME               20
   
#define CC_IPCUNDISTORTION_WIN_WIDTH                11
#define CC_IPCUNDISTORTION_WIN_HEIGHT               11
#define CC_IPCUNDISTORTION_ZERO_ZONE_WIDTH          -1
#define CC_IPCUNDISTORTION_ZERO_ZONE_HEIGHT         -1
#define CC_IPCUNDISTORTION_TERM_CRITERIA_MAX_ITER   30
#define CC_IPCUNDISTORTION_TERM_CRITERIA_EPSILON    0.1

#define CC_IPCUNDISTORTION_IMAGE_POINT_COLS         2
#define CC_IPCUNDISTORTION_IMAGE_POINT_TYPE         CV_32FC1
#define CC_IPCUNDISTORTION_OBJECT_POINT_COLS        3
#define CC_IPCUNDISTORTION_OBJECT_POINT_TYPE        CV_32FC1
#define CC_IPCUNDISTORTION_POINTS_COUNT_COLS        1
#define CC_IPCUNDISTORTION_POINTS_COUNT_TYPE        CV_32SC1

#define CC_IPCUNDISTORTION_INTRINSIC_MATRIX_ROWS    3
#define CC_IPCUNDISTORTION_INTRINSIC_MATRIX_COLS    3
#define CC_IPCUNDISTORTION_INTRINSIC_MATRIX_TYPE    CV_32FC1

#define CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_ROWS  5
#define CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_COLS  1
#define CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_TYPE  CV_32FC1




namespace CCF
{
    IPCUndistortion::IPCUndistortion():IPCalibration()
    {
        corners             = 0;
        distortionCoeffs    = 0;
        imagePoints         = 0;
        intrinsicMatrix     = 0;
        objectPoints        = 0;
        pointsCount         = 0;
    }

    IPCUndistortion::~IPCUndistortion()
    {
        Reset();
    }




    CvMat* IPCUndistortion::GetDistortionCoefficients() const
    {
        return distortionCoeffs;
    }


    CvMat* IPCUndistortion::GetIntrinsicMatrix() const
    {
        return intrinsicMatrix;
    }

    
    bool IPCUndistortion::LoadFromFile(const QString& file)
    {
        Reset();

		intrinsicMatrix  = cvCreateMat( CC_IPCUNDISTORTION_INTRINSIC_MATRIX_ROWS, CC_IPCUNDISTORTION_INTRINSIC_MATRIX_COLS, CC_IPCUNDISTORTION_INTRINSIC_MATRIX_TYPE );
        distortionCoeffs = cvCreateMat( CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_ROWS, CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_COLS, CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_TYPE );

        QByteArray b = file.toAscii();

        CvMat* t = (CvMat*) cvLoad(b.data()); // Qt not allow to do file.toAscii().data()
        
        // Copy datas to intrinsicMatrix
        for(int i=0; i<intrinsicMatrix->rows; i++)
            for(int j=0; j<intrinsicMatrix->cols; j++)
                CV_MAT_ELEM(*intrinsicMatrix, float, i, j) = CV_MAT_ELEM(*t, float, i, j);
        
        // Copy datas  to distortionCoeffs
        for(int i=0; i<distortionCoeffs->rows; i++)
            for(int j=0; j<distortionCoeffs->cols; j++)
                CV_MAT_ELEM(*distortionCoeffs, float, i, j) = CV_MAT_ELEM(*t, float, intrinsicMatrix->rows+i, j);

        return true;
    }

    void IPCUndistortion::Reset()
    {
        successCount = 0;
        frameCount   = 0;

        if(imagePoints)  cvReleaseMat(&imagePoints);   imagePoints  = 0;
        if(objectPoints) cvReleaseMat(&objectPoints);  objectPoints = 0;
        if(pointsCount)  cvReleaseMat(&pointsCount);   pointsCount  = 0;
        if(corners)      delete corners;               corners      = 0;

        if(intrinsicMatrix)  cvReleaseMat(&intrinsicMatrix);  intrinsicMatrix  = 0;
        if(distortionCoeffs) cvReleaseMat(&distortionCoeffs); distortionCoeffs = 0;

        newCalibration = true;
    }



    bool IPCUndistortion::SaveToFile(const QString& file)
    {
        CvMat* t = cvCreateMat(intrinsicMatrix->rows+distortionCoeffs->rows,intrinsicMatrix->cols,CC_IPCUNDISTORTION_INTRINSIC_MATRIX_TYPE);
        // init matrice
		cvSet(t,cvScalar(0.0));

        // Copy datas from intrinsicMatrix
        for(int i=0; i<intrinsicMatrix->rows; i++)
            for(int j=0; j<intrinsicMatrix->cols; j++)
                CV_MAT_ELEM(*t, float, i, j) = CV_MAT_ELEM(*intrinsicMatrix, float, i, j);
        
        // Copy datas  from distortionCoeffs
        for(int i=0; i<distortionCoeffs->rows; i++)
            for(int j=0; j<distortionCoeffs->cols; j++)
                CV_MAT_ELEM(*t, float, intrinsicMatrix->rows+i, j) = CV_MAT_ELEM(*distortionCoeffs, float, i, j);
        
        QByteArray b = file.toAscii();
        cvSave(b.data(),t);                 // Qt not allow to do file.toAscii().data()

        cvReleaseMat(&t);
        
        return true;
    }



    void IPCUndistortion::SetParameters(int _boardWidth, int _boardHeight, int _boardCount)
    {
        boardWidth  = _boardWidth;
        boardHeight = _boardHeight;
        boardCount  = _boardCount;

        newCalibration = true;
    }










    void IPCUndistortion::ProcessImage(Image& image)
    {
        if(newCalibration)
        {
            initialize();
            newCalibration = false;
        }

        
        if(successCount < boardCount && findCorners(image))
        {
            addCorners();
            successCount++;

            emit BoardMatched();
        }

        if(successCount == boardCount)
        {
            generateMatrices(image);
            emit ProcessTerminated();
        }
    }







    void IPCUndistortion::addCorners()
    {
        // Add corners to data
        for(int i=successCount*boardWidth*boardHeight, j=0; j<boardWidth*boardHeight; ++i,++j)
        {
            CV_MAT_ELEM( *imagePoints, float, i, 0 ) = corners[j].x;
			CV_MAT_ELEM( *imagePoints, float, i, 1 ) = corners[j].y;
			CV_MAT_ELEM( *objectPoints, float, i, 0 ) = j/boardWidth;
			CV_MAT_ELEM( *objectPoints, float, i, 1 ) = j%boardWidth;
			CV_MAT_ELEM( *objectPoints, float, i, 2 ) = 0.0f;

        }
        CV_MAT_ELEM( *pointsCount, int, successCount, 0 ) = boardWidth*boardHeight;
    }


    bool IPCUndistortion::findCorners(Image& image)
    {
        int cornerCount;
        // Skip every CC_IPCUNDISTORTION_SKIP_FRAME frames to allow user to move chessboard
        if( frameCount++ % CC_IPCUNDISTORTION_SKIP_FRAME == 0 )
        {
		    // Find chessboard corners:
            cvFindChessboardCorners( image, cvSize(boardWidth,boardHeight), corners,
			    &cornerCount, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );

		    // Get subpixel accuracy on those corners
            cvFindCornerSubPix( image, corners, cornerCount, cvSize( CC_IPCUNDISTORTION_WIN_WIDTH, CC_IPCUNDISTORTION_WIN_HEIGHT ), 
			    cvSize( CC_IPCUNDISTORTION_ZERO_ZONE_WIDTH, CC_IPCUNDISTORTION_ZERO_ZONE_HEIGHT ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, CC_IPCUNDISTORTION_TERM_CRITERIA_MAX_ITER, CC_IPCUNDISTORTION_TERM_CRITERIA_EPSILON ));

            //cvDrawChessboardCorners( image, cvSize(boardWidth,boardHeight), corners, cornerCount, found );
			    //cvShowImage( "Calibration", image );

            return cornerCount == boardWidth*boardHeight;
        }
        return false;
    }


    void IPCUndistortion::generateMatrices(Image& image)
    {
        // At this point we have all the chessboard corners we need
	    // Initiliazie the intrinsic matrix such that the two focal lengths
	    // have a ratio of 1.0
	    CV_MAT_ELEM( *intrinsicMatrix, float, 0, 0 ) = 1.0;
	    CV_MAT_ELEM( *intrinsicMatrix, float, 1, 1 ) = 1.0;

	    // Calibrate the camera
	    cvCalibrateCamera2( objectPoints, imagePoints, pointsCount, cvGetSize( image ), 
		    intrinsicMatrix, distortionCoeffs, NULL, NULL, CV_CALIB_FIX_ASPECT_RATIO );
    }



    void IPCUndistortion::initialize()
    {
        Reset();
        
        corners      = new CvPoint2D32f[ boardWidth*boardHeight ];

        imagePoints  = cvCreateMat( boardCount*boardWidth*boardHeight, CC_IPCUNDISTORTION_IMAGE_POINT_COLS, CC_IPCUNDISTORTION_IMAGE_POINT_TYPE);
        objectPoints = cvCreateMat( boardCount*boardWidth*boardHeight, CC_IPCUNDISTORTION_OBJECT_POINT_COLS, CC_IPCUNDISTORTION_OBJECT_POINT_TYPE);
        pointsCount  = cvCreateMat( boardCount, CC_IPCUNDISTORTION_POINTS_COUNT_COLS, CC_IPCUNDISTORTION_POINTS_COUNT_TYPE);

        intrinsicMatrix  = cvCreateMat( CC_IPCUNDISTORTION_INTRINSIC_MATRIX_ROWS, CC_IPCUNDISTORTION_INTRINSIC_MATRIX_COLS, CC_IPCUNDISTORTION_INTRINSIC_MATRIX_TYPE );
        distortionCoeffs = cvCreateMat( CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_ROWS, CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_COLS, CC_IPCUNDISTORTION_DISTORTIONS_COEFFS_TYPE );        
    }

} // namespace CCF