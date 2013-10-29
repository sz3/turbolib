#pragma once

#include "Event.h"
#include <functional>
#include <chrono>
#include <map>
#include <mutex>
#include <thread>

class SchedulerThread
{
public:
	SchedulerThread();
	~SchedulerThread();

	void shutdown();

	void schedule(const std::function<void()>& fun, unsigned millis);
	void schedulePeriodic(const std::function<void()>& fun, unsigned millis);

protected:
	void run();

	void waitForWork();
	bool doWork();

protected:
	bool _shutdown;
	std::thread _thread;

	std::mutex _mutex;
	std::multimap< std::chrono::system_clock::time_point,std::function<void()> > _funs;
	Event _notifyWork;
};
