#include "QuestiaEng/StateManager/StateManager.h"

#include "QuestiaEng/StateManager/State.h"
#include "QuestiaEng/Engine.h"

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
	isStateQueued = true;
	newStateName = stateName;
}
void StateManager::popState()
{
	if(stateStack.size())
	{
		isDelQueued = true;
		delIndex = stateStack.size() - 1;
	}
}
void StateManager::transitionState(std::string newState, std::string loadingState)
{
	isTransitionQueued = true;
	newStateName = newState;
	newTransitionName = loadingState;
}

void StateManager::deleteState(unsigned int index)
{
	//swap states until index is at the back
	for(unsigned int it = index; it < stackIndex; it++)
	{
		std::swap(stateStack[it], stateStack[it + 1]);
	}
	//delete state
	stateStack.pop_back();
	stackIndex--;
	isDelQueued = false;
}
void StateManager::createState(std::string stateName)
{
	if(stateMap.count(stateName))
	{
		stackIndex++;
		stateStack.push_back(std::unique_ptr<State>(stateMap[stateName]()));
		//init state
		stateStack.at(stackIndex)->eng = &eng;
		stateStack.at(stackIndex)->init();
	}
	isStateQueued = false;
}
void StateManager::makeTransition(std::string newState, std::string loadingState)
{
	//delete the top state
	if(stateStack.size()) {deleteState(stackIndex);};
	//create the transition state
	createState(loadingState);

	//initialize transition thread
	isStateLoading = true;
	thrFuture = thrPromise.get_future();
	loadingThread = std::thread([=]()
	{
		sf::Context context;
		State* state = stateMap[newState]();
		state->eng = &eng;
		state->init();
		thrPromise.set_value(state);
	});
	isTransitionQueued = false;
}

void StateManager::checkQueues()
{
	if(isDelQueued)
	{
		deleteState(delIndex);
	}
	if(isStateQueued)
	{
		createState(newStateName);
	}
	if(isTransitionQueued)
	{
		makeTransition(newStateName, newTransitionName);
	}
	checkLoading();
}
void StateManager::changeState(std::string stateName)
{
	//queue a state deletion
	popState();
	//add new state to front of stack
	pushState(stateName);
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
			stackIndex++;
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
		stateStack.at(stackIndex)->update(sf::Time::Zero);
	}
	checkQueues();
}
void StateManager::sProcessInput(std::u32string& inputText)
{
	if(stateStack.size())
	{
		stateStack.at(stackIndex)->processInput(inputText);
		inputText.clear();
	}
	checkQueues();
}
void StateManager::sDisplay()
{
	if(stateStack.size())
	{
		stateStack.at(stackIndex)->displayTextures();
	}
}

void StateManager::sUpdate(unsigned int offset)
{
	if(stateStack.size())
	{
		stateStack.at(stackIndex - offset)->update(sf::Time::Zero);
	}
	checkQueues();
}
void StateManager::sProcessInput(unsigned int offset, std::u32string& inputText)
{
	if(stateStack.size())
	{
		stateStack.at(stackIndex - offset)->processInput(inputText);
		inputText.clear();
	}
	checkQueues();
}
void StateManager::sDisplay(unsigned int offset)
{
	if(stateStack.size())
	{
		stateStack.at(stackIndex - offset)->displayTextures();
	}
}
