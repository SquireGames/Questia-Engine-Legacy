#include "QuestiaEng/StateManager/StateManager.h"

StateManager::StateManager(Engine& eng):
	eng(eng)
{
	std::cout << "DEBUG: StateManager Initialized" << std::endl;
}

StateManager::~StateManager()
{
	deleteState();
	std::cout << "DEBUG: StateManager Destroyed" << std::endl;
}

void StateManager::createState(State* state)
{
	this->stateStack.push(state);
	stateStack.top()->eng = &eng;
	stateStack.top()->init();
}

void StateManager::deleteState()
{
	while(!stateStack.empty())
	{
		delete stateStack.top();
		stateStack.pop();
	}
}

void StateManager::changeState(State* state)
{
	deleteState();
	createState(state);
}

void StateManager::processImputState(sf::Keyboard::Key key, bool isPressed)
{
	stateStack.top()->processImput(key, isPressed);
}

void StateManager::updateState()
{
	//TODO implement time into state updates
	stateStack.top()->update(sf::Time::Zero);
}

void StateManager::displayTexturesState()
{
	stateStack.top()->displayTextures();
}
