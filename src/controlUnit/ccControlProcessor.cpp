#include "ccControlProcessor.h"

namespace CCF
{
    ControlProcessor::ControlProcessor()
    {}

    ControlProcessor::~ControlProcessor()
    {}

    QList<int> ControlProcessor::GetParameter(short int /*parameterId*/, int /*specific*/)
    {
        return QList<int>();
    }

    bool ControlProcessor::SetParameter(short int /*parameterId*/, const QMap<int, Robot*>* /*value*/)
    {
        return false;
    }

    bool ControlProcessor::SetParameter(short int /*parameterId*/, const QMap<int, Track*>* /*value*/)
    {
        return false;
    }

    bool ControlProcessor::SetParameter(short int /*parameterId*/, QList<int>* /*value*/)
    {
        return false;
    }

    bool ControlProcessor::SetParameter(short int /*parameterId*/, int /*value*/)
    {
        return false;
    }

} // namespace CCF