#ifndef CC_ROBOT_CONTROLLER_HEADER
#define CC_ROBOT_CONTROLLER_HEADER

#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>


namespace CCF
{
    class RobotController: public QWidget
    {
        Q_OBJECT

        public:
            RobotController(QWidget* parent=0);
            int GetSpeed() const;

        public slots:

        signals:
            void Backward();
            void Forward();
            void RotateLeft();
            void RotateRight();
            void Stop();


        protected:            
            int speed;

            // Design
            void setupInterface();

            QSlider*     slSpeed;
            QPushButton* btnForward;
            QPushButton* btnBackward;
            QPushButton* btnRotateLeft;
            QPushButton* btnRotateRight;

        protected slots:
            void on_slSpeed_valueChanged(int);

        private:

    }; // class RobotController
} // namespace CCF
#endif // #ifndef CC_ROBOT_CONTROLLER_HEADER