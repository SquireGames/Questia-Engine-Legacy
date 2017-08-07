#include "QuestiaEng/Utl/Threads/Barrier.h"

utl::Barrier::Barrier(int count) noexcept:
	maxCount(count)
	, readyCounter(count)
	, generationToggler(false)
{

}

utl::Barrier::~Barrier()
{

}

void utl::Barrier::wait() noexcept
{
	std::unique_lock<std::mutex> lock(mutex);
	bool currentGeneration = generationToggler.getState();
	
	readyCounter.tick();
	
	if(readyCounter.isDone())
	{
		generationToggler.toggle();
		releaseCondVar.notify_all();
	}
	else
	{
		releaseCondVar.wait(lock, [&](){return currentGeneration != generationToggler.getState();});
	}
}
