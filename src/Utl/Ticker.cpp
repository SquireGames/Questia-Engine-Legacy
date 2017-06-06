#include "QuestiaEng/Utl/Ticker.h"

utl::Ticker::Ticker(unsigned int tickInterval):
    tickInterval(tickInterval)
{
	
}

void utl::Ticker::tick()
{
    if(currentTick < tickInterval)
    {
        currentTick++;
    }
}

void utl::Ticker::resetTicker()
{
    currentTick = 0;
}

bool utl::Ticker::isReady()
{
    return (currentTick >= tickInterval);
}

bool utl::Ticker::isDone()
{
    if(isReady())
    {
        resetTicker();
        return true;
    }
    return false;
}

bool utl::Ticker::pulse()
{
    tick();
    return isDone();
}


unsigned int utl::Ticker::getTick()
{
    return currentTick;
}

float utl::Ticker::getPercentage()
{
    return (static_cast<float>(currentTick) / static_cast<float>(tickInterval));
}
