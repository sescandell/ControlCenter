#ifndef CC_ROBOT_LAYER_HEADER
#define CC_ROBOT_LAYER_HEADER

#include "ccLayer.h"
#include "ccDisplayable.h"

#include "../robot/ccRobot.h"

#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>

#include <QtCore/QString>

namespace CCF
{
    class RobotLayer: public Layer
    {
        Q_OBJECT

        public:
            enum ParamId { PID_HAS_TRACK_ASSOCIATION,
                           PID_REPEAT,
                           PID_TRACK_FOLLOWER_STATE,
                           PID_TRACK_NAME,
                           PID_TRACK_STRING_STATE
            };
            enum TrackFollowerState { TF_RUNNING,
                                      TF_STOPPED,
                                      TF_PAUSED
            };

            RobotLayer(const Displayable* _object, QWidget* parent=0);

            LayerType GetLayerType() const;

            void SetHasTrackAssociated(bool value);
            bool SetParameter(int parameterId, int value);
            bool SetParameter(int parameterId, bool value);
            bool SetParameter(int parameterId, const QString& value);
            void SetRepeat(bool value);
            void SetTrackFollowerState(TrackFollowerState state);
            void SetTrackName(const QString& value);
            void SetStringState(const QString& value);

        public slots:
            void UpdatePosition();
            void UpdateOrientation();

        signals:
            void AddTrackRobotAssociationClicked(int /*robot identifier*/);
            void RemoveTrackRobotAssociationClicked(int /*robot identifier*/);
            void StartTrackFollowerClicked(int /*robot identifier*/);
            void StopTrackFollowerClicked(int /*robot identifier*/);
            void PauseTrackFollowerClicked(int /*robot identifier*/);
            void RepeatTrackFollowerClicked(int /*robot identifier*/, bool /* repeat state after click event*/);
            void RotationSpeedChanged(int /*robot identifier*/, int /*new value*/);
            void MainSpeedChanged(int /*robot identifier*/, int /*new value*/);


        protected:   
            static QString formatPortName(const QString& name);

            const Robot* robot() const;
            void updateInterface();

            bool hasTrackAssociated;
            bool repeat;
            TrackFollowerState trackFollowerState;

            // Design variables
            void setupSpecificInterface();

            QPushButton* btnRemoveTrack;
            QPushButton* btnNewTrack;
            QPushButton* btnStartFollowTrack;
            QPushButton* btnStopFollowTrack;
            QPushButton* btnPauseFollowTrack;
            QPushButton* btnRepeatFollowTrack;
            QLabel*      laPort;
            QLabel*      laPositionX;
            QLabel*      laPositionY;
            QLabel*      laOrientation;
            QLabel*      laTrackName;
            QLabel*      laTrackState;
            QSlider*     slRotationSpeed;
            QSlider*     slMovementSpeed;

        protected slots:
            void on_btnRemoveTrack_clicked();
            void on_btnNewTrack_clicked();
            void on_btnStartFollowTrack_clicked();
            void on_btnStopFollowTrack_clicked();
            void on_btnPauseFollowTrack_clicked();
            void on_btnRepeatFollowTrack_clicked();
            void on_slRotationSpeed_valueChanged(int);
            void on_slMovementSpeed_valueChanged(int);

        private:

    }; // class RobotLayer
} // namespace CCF
#endif // #ifndef CC_ROBOT_LAYER_HEADER