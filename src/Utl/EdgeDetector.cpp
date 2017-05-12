#include "QuestiaEng/Utl/EdgeDetector.h"


utl::EdgeDetector::EdgeDetector(bool initState):state(initState)
{
	
}

bool utl::EdgeDetector::risingEdge(bool newState)
{
	bool res = (!state && newState);
	state = newState;
	return res;
}

bool utl::EdgeDetector::fallingEdge(bool newState)
{
	bool res = (state && !newState);
	state = newState;
	return res;
}
