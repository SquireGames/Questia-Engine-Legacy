#include "QuestiaEng/Utl/Executor.h"

utl::Executor::Executor()
{
    //ctor
}

utl::Executor::~Executor()
{
    //dtor
}

void utl::Executor::addTask(std::string taskName, TaskType taskType, utl::Ticker ticker, std::function<void(float taskPercentage)> task)
{
    eventVector.push_back(std::make_pair(taskName, std::unique_ptr<Event>(new Event(taskType, ticker, task))));
}

void utl::Executor::processTasks()
{
    std::vector<std::string> toDelete;

    for(auto& event : eventVector)
    {
        TaskType& taskType = event.second->taskType;
        utl::Ticker& ticker = event.second->ticker;
        std::function<void(float taskPercentage)>& task = event.second->task;

        switch(taskType)
        {
        case TaskType::Continuous:
            ticker.tick();
            task(ticker.getPercentage());
            if(ticker.isReady())
            {
                toDelete.push_back(event.first);
            }
            break;
        case TaskType::Delayed:
            ticker.tick();
            if(ticker.isReady())
            {
                task(ticker.getPercentage());
                toDelete.push_back(event.first);
            }
            break;
        }
    }

    eventVector.erase(std::remove_if(eventVector.begin(), eventVector.end(), [&](std::pair<std::string, std::unique_ptr<Event>>& event)
    {
        return (std::find(toDelete.begin(), toDelete.end(), event.first) != toDelete.end());
    }), eventVector.end());
}

void utl::Executor::tryDelete(std::string taskName)
{
    auto taskIterator = eventVector.end();
    for(auto it = eventVector.begin(); it != eventVector.end(); it++)
    {
        if(taskName == (*it).first)
        {
            taskIterator = it;
            break;
        }
    }
    if(taskIterator != eventVector.end())
    {
        eventVector.erase(taskIterator);
    }
}

