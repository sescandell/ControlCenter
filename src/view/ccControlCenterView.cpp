#include "ccControlCenterView.h"

#include <cv.h>
#include <QtGui/QBrush>
#include <QtGui/QPen>

#include <QtCore/QDebug>

#define CC_CONTROL_CENTER_VIEW_IMAGE_CHANNELS   1
#define CC_CONTROL_CENTER_VIEW_IMAGE_DEPTH      IPL_DEPTH_8U

#define CC_CONTROL_CENTER_VIEW_QIMAGE_DEFAULT_FORMAT             QImage::Format_RGB32
#define CC_CONTROL_CENTER_VIEW_QHISTOGRAMIMAGE_DEFAULT_FORMAT    QImage::Format_RGB32

#define CC_CONTROL_CENTER_VIEW_CIRCLES_BRUSH            QBrush(Qt::blue)
#define CC_CONTROL_CENTER_VIEW_CIRCLES_PEN              QPen(Qt::red,1)

namespace CCF
{
    static int robotSize = 10;

    void ControlCenterView::SetRobotSize(int size)
    {
        robotSize = size;
    }

    ControlCenterView::ControlCenterView(const ControlCenter* _controlCenter, const QSize& _viewSize, const QSize& _histogramSize)
      :controlCenter(_controlCenter), viewSize(_viewSize), histogramSize(_histogramSize)
    {
        displayCircles  = false;
        displayImage    = true;
        image  = new Image();
        image->lock();
        image->CreateIplImage(viewSize.width(), viewSize.height(), CC_CONTROL_CENTER_VIEW_IMAGE_CHANNELS, CC_CONTROL_CENTER_VIEW_IMAGE_DEPTH);
        image->unlock();
        qImage = new QImage(viewSize,CC_CONTROL_CENTER_VIEW_QIMAGE_DEFAULT_FORMAT);
        qHistogramImage = new QImage(histogramSize,CC_CONTROL_CENTER_VIEW_QHISTOGRAMIMAGE_DEFAULT_FORMAT);
    }

    ControlCenterView::~ControlCenterView()
    {
        foreach(View* view, views)
            delete view;
        delete qHistogramImage;
        delete qImage;
        image->lock();
        delete image;
    }

    void ControlCenterView::AddView(int identifier, View* view)
    {
        views.insert(identifier,view);
        displayView.insert(identifier,true);
    }

    bool ControlCenterView::IsPointInImage(const QPoint& point) const
    {
        QPoint origin((viewSize.width()-qImage->width())/2,(viewSize.height()-qImage->height())/2);
        return point.x()>=origin.x() && point.x()<=origin.x()+qImage->width() &&
               point.y()>=origin.y() && point.y()<=origin.y()+qImage->height();
    }
    
    QPoint ControlCenterView::ConvertCoordinateToDisplayerSize(const QPoint& point) const
    {
        static QSize originalSize = controlCenter->GetVideoSize();
        QSize size = qImage->size();
        QPoint origin((viewSize.width()-qImage->width())/2,(viewSize.height()-qImage->height())/2);

        return QPoint(point.x()*(float)size.width() / (float)originalSize.width()+origin.x(),
                      point.y()*(float)size.height() / (float)originalSize.height()+origin.y());
    }

    QPoint ControlCenterView::ConvertCoordinateToVideoSourceSize(const QPoint& point) const
    {
        static QSize originalSize = controlCenter->GetVideoSize();
        QSize size = qImage->size();
        QPoint origin((viewSize.width()-qImage->width())/2,(viewSize.height()-qImage->height())/2);
        QPoint adjustedPoint = point - origin;

        return QPoint(adjustedPoint.x()* (float)originalSize.width()  / (float)size.width(),
                      adjustedPoint.y()* (float)originalSize.height() / (float)size.height() );
    }

    int ControlCenterView::ConvertDistanceToDisplayerDistance(int distance) const
    {
        // This function assumes that aspect ratio is kept
        static QSize originalSize = controlCenter->GetVideoSize();
        QSize size = qImage->size();

        return distance*size.width()/originalSize.width();
    }
    
    void ControlCenterView::DisplayerResized(QSize newSize)
    {
        // We take care that the aspect ratio is kept when resizing our displayer
        static QSize originalSize = controlCenter->GetVideoSize();
        static double ratio = (double)originalSize.width()/(double)originalSize.height();
        QSize imageSize;
        viewSize = newSize;
        if(ratio>=1) // landscape mode
        {
            imageSize.setWidth(newSize.width());
            imageSize.setHeight(newSize.width()/ratio);
            if(imageSize.height()>newSize.height())
            {
                imageSize.setHeight(newSize.height());
                imageSize.setWidth(newSize.height()*ratio);
            }
        }
        else // portrait mode
        {
            imageSize.setHeight(newSize.height());
            imageSize.setWidth(newSize.height()*ratio);
            if(imageSize.width()>newSize.width())
            {
                imageSize.setWidth(newSize.width());
                imageSize.setHeight(newSize.width()/ratio);
            }
        }
        image->lock();
        image->CreateIplImage(imageSize.width(), imageSize.height(), CC_CONTROL_CENTER_VIEW_IMAGE_CHANNELS, CC_CONTROL_CENTER_VIEW_IMAGE_DEPTH);
        image->unlock();
        delete qImage;
        qImage = new QImage(imageSize,CC_CONTROL_CENTER_VIEW_QIMAGE_DEFAULT_FORMAT);
    }

    void ControlCenterView::HistogramDisplayerResized(QSize newSize)
    {
        histogramSize = newSize;
        delete qHistogramImage;
        qHistogramImage = new QImage(histogramSize,CC_CONTROL_CENTER_VIEW_QHISTOGRAMIMAGE_DEFAULT_FORMAT);
    }

    Image* ControlCenterView::GetImage() const
    {
        return image;
    }

    void ControlCenterView::RemoveView(int identifier)
    {
        views.remove(identifier);
    }

    void ControlCenterView::SetDisplayImageActivated(int value)
    {
        displayImage = Qt::Checked==value;
    }

    void ControlCenterView::SetDisplayCirclesActivated(bool value)
    {
        displayCircles = value;
    }

    void ControlCenterView::SetDisplayView(int identifier,bool value)
    {
        displayView.insert(identifier,value);
    }

    void ControlCenterView::Draw(QPainter& painter)
    {
        painter.fillRect(0,0,viewSize.width(),viewSize.height(),QBrush(QColor(0,0,0)));
        if(displayImage)
        {
            //image->lock();
            if(image->tryLock())
            {
                IplImage* _image  = image->GetIplImage();
                int _widthStep    = _image->widthStep;
                int _width        = _image->width;
                int _height       = _image->height;
                char * _imageData = _image->imageData;
                int _channels     = image->GetChannelsCount();
                for(int y=0; y<_height; y++)
                {
                    for(int x=0;x<_width;x++)
                        qImage->setPixel(x,y,qRgb( _imageData[_channels*(y*_widthStep+x)],
                                 _imageData[_channels*(y*_widthStep+x)],
                                 _imageData[_channels*(y*_widthStep+x)]));
                }
                image->unlock();
            }
            // Draw the camera image in the center of the displayer
            painter.drawImage(QPoint((viewSize.width()-qImage->width())/2,(viewSize.height()-qImage->height())/2),*qImage);
        }

        // If we are in the add robot mode, we draw circles
        if(displayCircles)
        {
            painter.setBrush( CC_CONTROL_CENTER_VIEW_CIRCLES_BRUSH );
            painter.setPen(   CC_CONTROL_CENTER_VIEW_CIRCLES_PEN   );
            foreach(IPKPosition p, controlCenter->GetFoundPositions())
            {
                painter.drawEllipse(ConvertCoordinateToDisplayerSize(p),
                                    ConvertDistanceToDisplayerDistance(robotSize)/2,
                                    ConvertDistanceToDisplayerDistance(robotSize)/2);
            }
        }
        
        // Draw all activated views (robots and tracks)
        QMap<int, View*>::iterator i = views.begin();
        while (i != views.end()) 
        {
            if(displayView.value(i.key()))
                i.value()->Draw(painter);
            ++i;
        }
    }

    void ControlCenterView::DrawHistogram(QPainter& painter)
    {
        painter.fillRect(0,0,histogramSize.width(),histogramSize.height(),QBrush(QColor(255,255,255)));
        
        // There is a bug I do not succed to identify ...
        // Wherea I take the mutex, some one succed to access to <image>
        // and delete it ...
        return;

        // Initialize our histogram
        int values[256];
        for(int i=0;i<256;i++)
            values[i] = 0;
        //Compute the histogram
        if(image->tryLock())
        {
            if(image->GetChannelsCount()==1)
            {
                IplImage* _image  = image->GetIplImage();
                int _widthStep    = _image->widthStep;
                int _width        = _image->width;
                int _height       = _image->height;
                char * _imageData = _image->imageData;
                for(int y=0; y<_height; y++)
                {
                    for(int x=0;x<_width;x++)
                    {
                        values[_imageData[y*_widthStep+x]]++;
                    }
                }
                cvReleaseImage(&_image);
                image->unlock();
            }
            else
            {
                image->unlock();
                return;
            }
                // Find the biggest value
                int max = 0;
                for(int i=0;i<256;i++)
                    if(values[i]>max)
                        max = values[i];

                // Draw the histogram
                double heightRatio = (double)histogramSize.height()/(double)max;
                double widthRatio  = (double)histogramSize.width()/256.;

                painter.setBrush(Qt::NoBrush);
                painter.setPen(QColor(0,0,0));
                for(int i=0;i<256;i++)
                    painter.drawLine((histogramSize.width()-i)*widthRatio,histogramSize.height(),(histogramSize.width()-i)*widthRatio,values[i]*heightRatio);
        }
    }

} // namespace CCF
