#include "QuestiaEng/StateManager/StateManager.h"

StateManager::StateManager()
{
	std::cout << "DEBUG: StateManager Initialized" << std::endl;
}

StateManager::~StateManager()
{
	deleteState();
	std::cout << "DEBUG: StateManager Destroyed" << std::endl;
}

StateManager& StateManager::getInstance()
{
	static StateManager instance;
	return instance;
}

void StateManager::createState(State* state)
{
	this->stateStack.push(state);
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

void StateManager::updateState(sf::Time elapsedTime)
{
	stateStack.top()->update(elapsedTime);
}

void StateManager::displayTexturesState()
{
	stateStack.top()->displayTextures();
}
