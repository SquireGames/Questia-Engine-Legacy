#include "QuestiaEng/Utl/Threads/ThreadPool_Fixed.h"

ThreadPool_Fixed::ThreadPool_Fixed(unsigned int threads):
    maxThreads(std::min(threads, std::thread::hardware_concurrency()))
    , threadSync(maxThreads+1)
{
    for (unsigned int it = 0; it != maxThreads; it++)
    {
        threadPool.emplace_back(std::thread(&ThreadPool_Fixed::threadFunc, this));
    }
}

ThreadPool_Fixed::ThreadPool_Fixed():
    maxThreads(std::thread::hardware_concurrency())
    , threadSync(maxThreads+1)
{
    for (unsigned int it = 0; it != maxThreads; it++)
    {
        threadPool.emplace_back(std::thread(&ThreadPool_Fixed::threadFunc, this));
    }
}

ThreadPool_Fixed::~ThreadPool_Fixed()
{
    kill();
}

void ThreadPool_Fixed::addTask(std::function<void()> task)
{
    taskPool.emplace_back(TaskObj(task));
}

void ThreadPool_Fixed::threadFunc()
{
    while(true)
    {
        //wait until flag to start something
        threadSync.wait();
	
        if(killAll)
        {
            return;
        }
        for(auto& taskObj : taskPool)
        {
            taskObj.taskMutex->lock();
            if(taskObj.runTask)
            {
                taskObj.runTask = false;
                taskObj.taskMutex->unlock();
                taskObj.task();
            }
            else
            {
                taskObj.taskMutex->unlock();
            }
        }

        //make sure all threads finish
        threadSync.wait();
    }
}



void ThreadPool_Fixed::runTasks()
{
    //set the tasks to run
    for(auto& taskObj : taskPool)
    {
        taskObj.taskMutex->lock();
        taskObj.runTask = true;
        taskObj.taskMutex->unlock();
    }

    //start threads
    threadSync.wait();

    //wait for threads to finish
    threadSync.wait();
}

void ThreadPool_Fixed::kill()
{
    if(threadPool.size() > 0)
    {
        killAll = true;

        //wait for tasks to finish
        threadSync.wait();

        //join all threads
        for(auto& thread : threadPool)
        {
            thread.join();
        }
        threadPool.clear();
    }
}
