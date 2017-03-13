#include "QuestiaEng/Utl/Toggler.h"

utl::Toggler::Toggler(bool initValue):
    value(initValue)
{
    //ctor
}

utl::Toggler::~Toggler()
{
    //dtor
}

bool utl::Toggler::toggle()
{
    value = !value;
    return value;
}

bool utl::Toggler::getState()
{
    return value;
}
