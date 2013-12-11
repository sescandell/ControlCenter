#ifndef CC_TRACK_CREATOR_HEADER
#define CC_TRACK_CREATOR_HEADER

#include <QtGui/QWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>

#include <QtCore/QString>


namespace CCF
{
    class TrackCreator: public QWidget
    {
        Q_OBJECT

        public:
            struct TrackCreationParameters
            {
                QString name;
            };

            TrackCreator(QWidget* parent=0);

            TrackCreationParameters GetParameters() const;

        public slots:
            void setVisible(bool visible);

        signals:

        protected:

            // Design variables
            void setupInterface();

            QLineEdit*   edName;

        protected slots:

        private:

    }; // class TrackCreator
} // namespace CCF
#endif // #ifndef CC_ROBOT_CREATOR_HEADER