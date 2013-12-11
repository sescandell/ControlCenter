#ifndef CC_IMAGEPROCESSOR_HEADER
#define CC_IMAGEPROCESSOR_HEADER

#include <QObject>
#include <QList>
#include <QPoint>

#include "ccImage.h"

namespace CCF
{
    typedef QPoint             IPKPosition;
    typedef QList<IPKPosition> IPKListPositions;

    class ImageProcessor: public QObject
    {
        Q_OBJECT

        public:
            ImageProcessor();
            virtual ~ImageProcessor();

            virtual void ProcessImage(Image& image) = 0;
            virtual void Reset() = 0;

        public slots:
            

        signals:

        protected:

        private:

    }; // class ImageProcessor
} // namespace CCF

#endif // #ifndef CC_IMAGEPROCESSOR_HEADER