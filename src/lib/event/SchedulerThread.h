/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "Event.h"
#include <functional>
#include <chrono>
#include <queue>
#include <mutex>
#include <thread>

class SchedulerThread
{
protected:
	struct Funct
	{
		std::chrono::system_clock::time_point time;
		std::function<void()> fun;

		bool operator<(const Funct& other) const
		{
			return time > other.time;
		}
	};

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
	std::priority_queue<Funct> _funs;
	Event _notifyWork;
};
