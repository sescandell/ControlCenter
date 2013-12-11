#include "ccControlUnit.h"

#include "ccCPTracker.h"


namespace CCF
{
    static const short int CC_CONTTROL_UNIT_SEM_COUNT_GET_SYNCHRO    = 1;
    static const short int CC_CONTROL_UNIT_SEM_PUT_SYNCHRO           = 1;

    static const short int CC_CONTROL_UNIT_PUT_SYNCHRO_IP_RESSOURCES = 1;

    ControlUnit::ControlUnit(QSemaphore* _semSynchroImage)
        :QThread(), mainProcessor(0), semSynchroImage(_semSynchroImage)
    {
        start();
        synchro.acquire(CC_CONTTROL_UNIT_SEM_COUNT_GET_SYNCHRO);
    }

    ControlUnit::~ControlUnit()
    {
        quit();
        wait();
        delete mainProcessor;
    }

    QList<int> ControlUnit::GetParameter(short int parameterId, int specific)
    {
        if(mainProcessor)
            return mainProcessor->GetParameter(parameterId,specific);
        else
            return QList<int>();
    }

    bool ControlUnit::SetParameter(short int parameterId, const QMap<int, Robot*>* value)
    {
        return 0!=mainProcessor && mainProcessor->SetParameter(parameterId,value);
    }

    bool ControlUnit::SetParameter(short int parameterId, const QMap<int, Track*>* value)
    {
        return 0!=mainProcessor && mainProcessor->SetParameter(parameterId,value);
    }

    bool ControlUnit::SetParameter(short int parameterId, QList<int>* value)
    {
        return 0!=mainProcessor && mainProcessor->SetParameter(parameterId,value);
    }

    bool ControlUnit::SetParameter(short int parameterId, int value)
    {
        return 0!=mainProcessor && mainProcessor->SetParameter(parameterId,value);
    }

    void ControlUnit::run()
    {
        mainProcessor = new CPTracker(semSynchroImage);

        connect(this,SIGNAL(NewPositionsAvailable(IPKListPositions)), ((CPTracker*)mainProcessor), SLOT(OnNewPositionsAvailable(IPKListPositions)));

        connect(((CPTracker*)mainProcessor), SIGNAL(EndOfLapReached(int)),   this, SIGNAL(EndOfLapReached(int)), Qt::DirectConnection);
        connect(((CPTracker*)mainProcessor), SIGNAL(EndOfTrackReached(int)), this, SIGNAL(EndOfTrackReached(int)), Qt::DirectConnection);
        
        // We signal to the image processing that we are ready to catch signals to process control
        semSynchroImage->release(CC_CONTROL_UNIT_PUT_SYNCHRO_IP_RESSOURCES);

        synchro.release(CC_CONTROL_UNIT_SEM_PUT_SYNCHRO);

        exec();
    }
} // namespace CCF