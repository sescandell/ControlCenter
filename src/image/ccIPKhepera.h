#ifndef CC_IPKHEPERA_HEADER
#define CC_IPKHEPERA_HEADER

#include "ccImageProcessor.h"
#include "ccImage.h"

#include <QtCore/QSize>

namespace CCF
{
    class IPKhepera: public ImageProcessor
    {
        Q_OBJECT

        public:
            IPKhepera();
            ~IPKhepera();

            void SetKheperaSize(int _size);
            void ProcessImage(Image& image);
            void Reset();

        signals:
            void CirclesFound(IPKListPositions);

        protected:
            IplImage* resizeImage(const Image& image) const;

            int minRadius;
            int maxRadius;

        private:
            
    };
} // namespace CCF

#endif // #ifndef CC_IPKHEPERA_HEADER