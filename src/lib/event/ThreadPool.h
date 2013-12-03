#pragma once

#include "CounterEvent.h"
#include "Event.h"
#include "tbb/concurrent_queue.h"
#include <list>
#include <memory>
#include <thread>

class ThreadPool
{
public:
	ThreadPool(unsigned numThreads = std::thread::hardware_concurrency());
	~ThreadPool();

	bool start();
	void stop();

	void execute(std::function<void()> fun);
	size_t queued() const;

protected:
	void run();

protected:
	std::atomic<bool> _running;
	unsigned _numThreads;
	std::list<std::thread> _threads;

	CounterEvent _notifyRunning;
	Event _notifyWork;
	tbb::concurrent_queue< std::function<void()> > _queue;
};
