#include "QuestiaEng/StateManager/State.h"

State::State(std::shared_ptr<void> params):
	pParams(std::move(params))
{
	
}

State::~State()
{

}
