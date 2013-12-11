#ifndef CC_ROBOT_CREATOR_HEADER
#define CC_ROBOT_CREATOR_HEADER

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>

#include <QtCore/QString>
#include <QtCore/QMap>

#include "../robot/ccCommunicator.h"

namespace CCF
{
    class RobotCreator: public QWidget
    {
        Q_OBJECT

        public:
            struct RobotCreationParameters
            {
                QString address;
                Communicator::Mode communicatorMode;
                QString interfaceName;
                QString name;                
            };

            RobotCreator(QWidget* parent=0);

            RobotCreationParameters GetParameters() const;

        public slots:
            void setVisible(bool visible);

        signals:

        protected:
            static QString formatPortName(const QString& name);

            QMap<QString, QString> portMapping;

            // Design
            void setupInterface();

            QLineEdit*   edName;
            QLineEdit*   edAddress;
            QGroupBox*   gbMultiRobot;
            QComboBox*   cbPort;


        protected slots:

        private:

    }; // class RobotCreator
} // namespace CCF
#endif // #ifndef CC_ROBOT_CREATOR_HEADER