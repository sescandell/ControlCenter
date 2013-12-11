#include "ccControlCenter.h"

#include <QMetaType>
#include <QScopedArrayPointer>

#include "../robot/ccKhepera.h"
#include "../controlUnit/ccCPTracker.h"
#include "../controlUnit/ccCPFrontFinder.h"
#include "../controlUnit/ccCPTrackFollower.h"


namespace CCF
{
    ControlCenter::ControlCenter()
        :robotSize(10)
    {
        qRegisterMetaType<IPKListPositions>("IPKListPositions");    // we need that to signals/slots can work with IPKhepera
        
        imageProcessing = new ImageProcessing(&semSynchroImageControl);

        controlUnit = new ControlUnit(&semSynchroImageControl);
        controlUnit->SetParameter(CPTracker::PARAM_ID_TRACKER_SET_ROBOTS_LIST, &robots);
        controlUnit->SetParameter(CPFrontFinder::PARAM_ID_FRONT_FINDER_SET_ROBOTS_LIST, &robots);
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_LIST, &robots);
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_SET_TRACKS_LIST, &tracks);

        connect(controlUnit, SIGNAL(EndOfTrackReached(int)), this, SIGNAL(EndOfTrackReached(int)), Qt::DirectConnection);
        connect(controlUnit, SIGNAL(EndOfTrackReached(int)), this, SLOT(onEndOfTrackReached(int)));
        //connect(controlUnit, SIGNAL(EndOfLapReached(int)), this, SLOT(onEndOfLapReached(int)));

        connect(imageProcessing, SIGNAL(ImageProcessed()),                    this, SIGNAL(ImageProcessed()) , Qt::DirectConnection);
        connect(imageProcessing, SIGNAL(NewPositionsFound(IPKListPositions)), this, SLOT(OnNewPositionsFound(IPKListPositions)));
        connect(imageProcessing, SIGNAL(NewPositionsFound(IPKListPositions)), controlUnit, SIGNAL(NewPositionsAvailable(IPKListPositions)), Qt::DirectConnection);
        connect(imageProcessing, SIGNAL(BoardMatched()),                      this, SIGNAL(BoardMatched()), Qt::DirectConnection);
        connect(imageProcessing, SIGNAL(CalibrationEnded()),                  this, SIGNAL(CalibrationEnded()), Qt::DirectConnection);
    }

    ControlCenter::~ControlCenter()
    {
        disconnect(imageProcessing, SIGNAL(NewPositionsFound(IPKListPositions)), controlUnit, SIGNAL(NewPositionsAvailable(IPKListPositions)));
        delete controlUnit;

        imageProcessing->Stop();
        imageProcessing->wait();        

        foreach(Track* track, tracks)
            delete track;
        foreach(Robot* robot, robots)
            delete robot;
        foreach(Communicator* communicator, robotCommunicators)
            delete communicator;
    }

    QList<int> ControlCenter::GetAssociatedRobotsToTrack(int trackId)
    {
        return controlUnit->GetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_GET_ASSOCIATED_ROBOTS,trackId);
    }

    int ControlCenter::GetFPS()
    {
        return imageProcessing->GetFPS();
    }

    const Track* ControlCenter::AddTrack(const QString& name, const Track* source)
    {
        Track* track = new Track(name);
        // TODO : utiliser les iterateurs
        for(int i=0; i<source->size(); i++)
        {
            track->append((*source)[i]);
        }
        tracks.insert(track->GetIdentifier(),track);

        return track;
    }

    const Robot* ControlCenter::AddKhepera(const QString& name, const QPoint& position, const QString& interfaceName, Communicator::Mode communicatorMode, const QString& address)
    {
        if(robotCommunicators.contains(interfaceName))
        {
            if(Communicator::COMMUNICATOR_MODE_SINGLE_ROBOT==communicatorMode || communicatorMode!=robotCommunicators[interfaceName]->GetMode())
                return 0;
        }
        else
        {
            Communicator* communicator = new Communicator(interfaceName,communicatorMode);
            communicator->Connect();
            robotCommunicators.insert(interfaceName, communicator);
        }
        Robot* robot = new Khepera(name,position, robotCommunicators[interfaceName], address);
        if(communicatorMode==Communicator::COMMUNICATOR_MODE_MULTI_ROBOT)
            robotCommunicators[interfaceName]->AddRobot(robot);

        robots.insert(robot->GetIdentifier(), robot);

        return robot;
    }

    void ControlCenter::InitializeUndistortionCalibration(int boardWidth, int boardHeight, int boardCount)
    {
        imageProcessing->InitializeUndistortion(boardWidth,boardHeight,boardCount);
    }

    const Robot* ControlCenter::GetRobot(int robotId) const
    {
        return robots.value(robotId, 0);
    }

    const IPKListPositions& ControlCenter::GetFoundPositions() const
    {
        return lastPositions;
    }

	QSize& ControlCenter::GetVideoSize() const
	{
		return imageProcessing->GetVideoSourceSize();
	}

    void ControlCenter::LoadCalibrationFromFile(QString& file)
    {
        imageProcessing->LoadUndistortionParametersFromFile(file);
    }


    void ControlCenter::MoveRobot(int identifier, int speedLeft, int speedRight)
    {
        if(robots.contains(identifier))
        {
            Khepera* robot = (Khepera*)(robots[identifier]);
            robot->lock();
            //robot->ResetPositionCounters();
            robot->SetSpeed(speedLeft,speedRight);
            robot->unlock();
        }
    }

    bool ControlCenter::RemoveRobot(int identifier)
    {
        if(!robots.contains(identifier)) return false;
        // Ask the robot to stop following a track if it is
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_REMOVE_ASSOCIATION,identifier);
        // Delete the robot
        Robot* robot = robots.take(identifier);
        robot->lock();
        // Delete the communicator if necessary
        Communicator* communicator = robotCommunicators[robot->GetInterfaceName()];
        if(Communicator::COMMUNICATOR_MODE_SINGLE_ROBOT==communicator->GetMode())
        {
            robotCommunicators.remove(robot->GetInterfaceName());
            delete communicator;
        }
        else
        {
            communicator->RemoveRobot(robot);
        }
        delete robot;

        return true;
    }

    bool ControlCenter::RemoveTrack(int identifier)
    {
        if(!tracks.contains(identifier)) return false;
        // Stop all robots following the current track
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_REMOVE_ALL_TRACK_ASSOCIATION,identifier);
        // Delete the track
        Track* track = tracks.take(identifier);
        track->lock();
        delete track;
        
        return true;
    }

    void ControlCenter::SaveCalibrationToFile(QString& file)
    {
        imageProcessing->SaveUndistortionParametersToFile(file);
    }

    void ControlCenter::OnSetCalibrationActivated(int value)
    {
        SetCalibrationActivated(Qt::Checked==value);
    }

    void ControlCenter::OnSetNormalizationActivated(int value)
    {
        SetNormalizationActivated(Qt::Checked==value);
    }

    void ControlCenter::SetCalibrationActivated(bool value)
    {
        imageProcessing->SetCalibrationActivated(value);
    }

    void ControlCenter::SetNormalizationActivated(bool value)
    {
        imageProcessing->SetNormalizationActivated(value);
    }

    void ControlCenter::SetDisplayActivated(int value)
    {
        imageProcessing->SetDisplayActivated(Qt::Checked==value);
    }

    void ControlCenter::SetDisplayableImage(Image* image)
    {
        imageProcessing->SetDisplayableImage(image);        
    }

    void ControlCenter::SetKheperaSize(int kSize)
    {
        robotSize = kSize;
        imageProcessing->SetKheperaSize(kSize);
        // We can do better ...
        controlUnit->SetParameter(CPTracker::PARAM_ID_TRACKER_SET_ROBOTS_SIZE,kSize);
        controlUnit->SetParameter(CPFrontFinder::PARAM_ID_FRONT_FINDER_SET_ROBOTS_SIZE,kSize);
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_SET_ROBOTS_SIZE,kSize);
    }

    void ControlCenter::StartCalibration()
    {
        imageProcessing->SetImageProcessing(IPC_UNDISTORTION);
    }

    void ControlCenter::StartTrackingKheperaProcess()
    {
        imageProcessing->SetImageProcessing(IP_KHEPERA);
    }

    void ControlCenter::StopProcessing()
    {
        imageProcessing->SetImageProcessing(IP_NONE);
    }

    void ControlCenter::OnNewPositionsFound(IPKListPositions positions)
    {
        // Save circles
        lastPositions = positions;
        emit NewPositionsFindCount(positions.size());
    }

    bool ControlCenter::PositionExists(const QPoint& position) const
    {
        QPoint distance;
        foreach(IPKPosition point, lastPositions)
        {
            distance = point - position;          
            if(distance.manhattanLength()<robotSize/2) return true;
        }
        
        return false;
    }

    void ControlCenter::AddRobotToFollowTrack(int robotId, int trackId, int lapsCount)
    {
        QList<int> params;
        params << robotId << trackId << lapsCount;
        controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_ADD_ASSOCIATION,&params);
    }

    bool ControlCenter::StartRobotFollowingTrack(int robotId)
    {
        return controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_START_FOLLOWER,robotId);
    }

    bool ControlCenter::StopRobotFollowingTrack(int robotId)
    {
        return controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_STOP_FOLLOWER,robotId);
    }

    bool ControlCenter::PauseRobotFollowingTrack(int robotId)
    {
        return controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_PAUSE_FOLLOWER,robotId);
    }

    bool ControlCenter::DefineTrackFollowingLapsTodo(int robotId, int value)
    {
        QList<int> params;
        params << robotId;

        if(ControlCenter::CC_CONTROL_CENTER_INFINITE_LAPS==value)
            params << CPTrackFollower::CC_CP_TRACK_FOLLOWER_INFINITE_LAPS;
        else
            params << value;

        return controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_SET_LAPS_TODO,&params);
    }

    QString ControlCenter::GetTrackName(int trackId) const
    {
        if(tracks.contains(trackId))
        {
            return tracks[trackId]->GetName();
        }
        else
        {
            return QString::fromUtf8("Unknow");
        }
    }

    bool ControlCenter::RemoveRobotTrackAssociation(int robotId)
    {
        return controlUnit->SetParameter(CPTrackFollower::PARAM_ID_TRACK_FOLLOWER_REMOVE_ASSOCIATION,robotId);
    }

    void ControlCenter::onEndOfTrackReached(int robotId)
    {
        StopRobotFollowingTrack(robotId);
    }

    void ControlCenter::SetRobotRotationSpeed(int robotId, int value)
    {
        if(!robots.contains(robotId)) return;

        robots[robotId]->SetUserRotationSpeed(value);
    }

    void ControlCenter::SetRobotMainSpeed(int robotId, int value)
    {
        if(!robots.contains(robotId)) return;

        robots[robotId]->SetUserSpeed(value);
    }
} // namespace CCF