#ifndef CC_CP_TRACK_FOLLOWER_HEADER
#define CC_CP_TRACK_FOLLOWER_HEADER

#include <QtCore/QList>
#include <QtCore/QMap>


#include "ccControlProcessor.h"
#include "../robot/ccRobot.h"
#include "../track/ccTrack.h"

namespace CCF
{
    class CPTrackFollower: public ControlProcessor
    {
        Q_OBJECT

        public:
            static const int       CC_CP_TRACK_FOLLOWER_INFINITE_LAPS         = -1;
            
            static const short int PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_LIST    = 0x0100;
            static const short int PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_SIZE    = 0x0200;
            static const short int PARAM_ID_TRACK_FOLLOWER_SET_TRACKS_LIST    = 0x0300;
            static const short int PARAM_ID_TRACK_FOLLOWER_ADD_ASSOCIATION    = 0x0400;
            static const short int PARAM_ID_TRACK_FOLLOWER_REMOVE_ASSOCIATION = 0x0500;
            static const short int PARAM_ID_TRACK_FOLLOWER_START_FOLLOWER     = 0x0600;
            static const short int PARAM_ID_TRACK_FOLLOWER_STOP_FOLLOWER      = 0x0700;
            static const short int PARAM_ID_TRACK_FOLLOWER_PAUSE_FOLLOWER     = 0x0800;
            static const short int PARAM_ID_TRACK_FOLLOWER_REMOVE_ALL_TRACK_ASSOCIATION = 0x0900;
            static const short int PARAM_ID_TRACK_FOLLOWER_SET_LAPS_TODO      = 0x0A00;

            static const short int PARAM_ID_TRACK_FOLLOWER_GET_ASSOCIATED_ROBOTS = 0x0100;

            CPTrackFollower();
            ~CPTrackFollower();
            
            void RunProcess();

            QList<int> GetParameter(short int parameterId, int specific);

            bool SetParameter(short int parameterId, const QMap<int, Robot*>* value);
            bool SetParameter(short int parameterId, const QMap<int, Track*>* value);
            bool SetParameter(short int parameterId, QList<int>* value);
            bool SetParameter(short int parameterId, int value);

        signals:
            void EndOfLapReached(int /* Robot identifier */);
            void EndOfTrackReached(int /* Robot identifier */);

        protected:
            void addAssociation(int robotId, int trackId, int lapsCount);
            double computeAngle(const Robot* robot, const Track* track) const;
            double computeCorrectiveOrientation(const Robot* robot, const Track* track) const;
            int computeSpeed(const Robot* robot, const Track* track) const;
            void correctRobotTrajectory(Robot* robot, const Track* track) const;
            bool destinationReached(const Robot* robot, const Track* track) const;
            void initializeNextDestination(const Robot* robot, const Track* track);
            void moveRobot(Robot* robot, const Track* track) const;
            void removeAllTrackAssociation(int trackId);
            void removeAssociation(int robotId);
            bool robotOnGoodTrajectory(const Robot* robot, const Track* track) const;
            void startFollower(int robotId);
            void pauseFollower(int robotId);
            void stopFollower(int robotId);
            void setLapsTodo(int robotId, int value);

            struct TFState
            {                
                int  lapsToDo;
                int  lapsDone;
                bool enable;
                int  trackPointIndex;
            }; // TFState

            QMap<int /*Robot identifier*/, int /*Track identifier*/>  robotsTracks;
            QMap<int /*Robot identifier*/, TFState > robotsStates;
            const QMap<int /*Robot identifier*/, Robot*>* robots;
            const QMap<int /*Track identifier*/, Track*>* tracks;

        private:
    };
} // namespace CCF
#endif // #ifndef CC_CP_TRACK_FOLLOWER_HEADER