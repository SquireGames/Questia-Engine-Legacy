#include "QuestiaEng/StateManager/StateManager.h"

#include "QuestiaEng/StateManager/State.h"

StateManager::StateManager(Engine& eng):
	eng(eng)
{
	stateStack.reserve(3);
	std::cout << "DEBUG: StateManager Initialized" << std::endl;
}
StateManager::~StateManager()
{
	//make sure thread is not active for graceful exit
	if(loadingThread.joinable())
	{
		loadingThread.join();
	}
	
	std::cout << "DEBUG: StateManager Destroyed" << std::endl;
}

void StateManager::reg(std::string name, std::function<State*()> state)
{
	stateMap[name] = state;
}

void StateManager::pushState(std::string stateName)
{
	if(stateMap.count(stateName))
	{
		stateStack.push_back(std::unique_ptr<State>(stateMap[stateName]()));
		//init state
		stateStack.at(stateStack.size() - 1)->eng = &eng;
		stateStack.at(stateStack.size() - 1)->init();
	}
}
void StateManager::popState()
{
	if(stateStack.size())
	{
		isDelQueued = true;
		delIndex = stateStack.size() - 1;
	}
}
void StateManager::deleteState(unsigned int index)
{
	//swap states until index is at the back
	for(unsigned int it = index; it < (stateStack.size() - 1); it++)
	{
		std::swap(stateStack[it], stateStack[it + 1]);
	}
	//delete state
	stateStack.pop_back();
	isDelQueued = false;
}
void StateManager::checkDelQueue()
{
	if(isDelQueued)
	{
		deleteState(delIndex);
	}
}
void StateManager::changeState(std::string stateName)
{
	//queue a state deletion
	popState();
	//add new state to front of stack
	pushState(stateName);
}
void StateManager::transitionState(std::string newState, std::string loadingState)
{
	//queue state deletion
	popState();
	//load the transition state
	pushState(loadingState);

	//initialize transition thread
	isStateLoading = true;
	thrFuture = thrPromise.get_future();
	loadingThread = std::thread([=]()
	{
		State* state = stateMap[newState]();
		state->eng = &eng;
		state->init();
		thrPromise.set_value(state);
	});
}

void StateManager::checkLoading()
{
	if(isStateLoading)
	{
		std::future_status threadStatus = thrFuture.wait_for(std::chrono::milliseconds(0));
		if(threadStatus == std::future_status::ready)
		{
			isStateLoading = false;
			loadingThread.join();
			popState();
			stateStack.push_back(std::unique_ptr<State>(thrFuture.get()));
			thrPromise = std::promise<State*>();
			thrFuture = std::future<State*>();
			std::cout << "Thread done!" << std::endl;
		}
	}
}

void StateManager::sUpdate()
{
	if(stateStack.size())
	{
		stateStack.at(stateStack.size() - 1)->update(sf::Time::Zero);
	}
	checkDelQueue();
	checkLoading();
}
void StateManager::sProcessInput(std::string input)
{
	if(stateStack.size())
	{
		stateStack.at(stateStack.size() - 1)->processInput(sf::Keyboard::Unknown, false);
	}
	checkDelQueue();
	checkLoading();
}
void StateManager::sDisplay()
{
	if(stateStack.size())
	{
		stateStack.at(stateStack.size() - 1)->displayTextures();
	}
}
