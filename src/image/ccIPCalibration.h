#ifndef CC_IPCALIBRATION_HEADER
#define CC_IPCALIBRATION_HEADER

#include <QObject>

#include "ccImageProcessor.h"

namespace CCF
{
    class IPCalibration: public ImageProcessor
    {
        Q_OBJECT

        public:
            IPCalibration();
            virtual ~IPCalibration();

            virtual bool LoadFromFile(const QString& file) = 0;
            virtual void ProcessImage(Image& image)        = 0;
            virtual void Reset()                           = 0;
            virtual bool SaveToFile(const QString& file)   = 0;

        public slots:
            

        signals:
            void ProcessTerminated();

        protected:
            bool newCalibration;

        private:

    }; // class IPCalibration
} // namespace CCF

#endif // #ifndef CC_IPCALIBRATION_HEADER