#include "ccDisplayable.h"

namespace CCF
{
    static int identifierCounter = 0;

    Displayable::Displayable(const QString& _name)
        :QObject(), identifier(identifierCounter++), name(_name)
    {}

    Displayable::~Displayable()
    {}

    int Displayable::GetIdentifier() const
    {
        return identifier;
    }

    const QString& Displayable::GetName() const
    {
        return name;
    }
} // namespace CCF