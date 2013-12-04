#pragma once

#include "IExecutor.h"
#include "CounterEvent.h"
#include "Event.h"
#include "tbb/concurrent_queue.h"
#include <list>
#include <memory>
#include <thread>

class MultiThreadedExecutor : public IExecutor
{
public:
	MultiThreadedExecutor(unsigned numThreads = std::thread::hardware_concurrency());
	~MultiThreadedExecutor();

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
