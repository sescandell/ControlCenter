#include "ccTrack.h"

namespace CCF
{
    Track::Track(const QString& _name)
        :Displayable(_name), QPolygon(), QMutex()
    {}
} // namespace CCF