/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "concurrent/monitor.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace turbo {
class deadline_thread
{
protected:
	struct job
	{
		std::chrono::system_clock::time_point time;
		std::function<void()> fun;

		bool operator<(const job& other) const
		{
			return time > other.time;
		}
	};

	static std::chrono::system_clock::time_point now()
	{
		return std::chrono::system_clock::now();
	}

	static long long millisFromNow(const std::chrono::system_clock::time_point& when)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(when - now()).count();
	}

public:
	deadline_thread();
	~deadline_thread();

	void shutdown();

	void schedule(const std::function<void()>& fun, unsigned ms);
	void schedule_repeat(const std::function<void()>& fun, unsigned ms);

protected:
	void run();

	void waitForWork();
	bool doWork();

protected:
	std::atomic<bool> _shutdown;
	std::thread _thread;

	std::mutex _mutex;
	std::priority_queue<job> _jobs;
	turbo::monitor _notifyWork;
};

inline deadline_thread::deadline_thread()
	: _shutdown(false)
	, _thread(std::bind(&deadline_thread::run, this))
{
}

inline deadline_thread::~deadline_thread()
{
	shutdown();
}

inline void deadline_thread::shutdown()
{
	if (_shutdown.exchange(true))
		return;
	_notifyWork.notify_all();
	_thread.join();
}

inline void deadline_thread::run()
{
	while (!_shutdown)
	{
		waitForWork();
		while (doWork());
	}
}

inline void deadline_thread::waitForWork()
{
	unsigned interval = ~0;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (!_jobs.empty())
		{
			const job& top = _jobs.top();
			long long duration = millisFromNow(top.time);
			interval = (duration < 0)? 0 : duration;
		}
	}
	_notifyWork.wait_for(interval);
}

inline bool deadline_thread::doWork()
{
	if (_shutdown)
		return false;

	std::function<void()> work;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_jobs.empty())
			return false;

		const job& top = _jobs.top();
		if (millisFromNow(top.time) > 0) // would like to use operator<, but apparently it doesn't work?
			return false;

		work = top.fun;
		_jobs.pop();
	}
	work();
	return true;
}

inline void deadline_thread::schedule(const std::function<void()>& fun, unsigned ms)
{
	std::chrono::system_clock::time_point when = now() + std::chrono::milliseconds(ms);
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_jobs.push( {when, fun} );
	}
	_notifyWork.notify_one();
}

inline void deadline_thread::schedule_repeat(const std::function<void()>& fun, unsigned ms)
{
	std::function<void()> wrapper( [=] () { fun(); this->schedule_repeat(fun, ms); } );
	schedule(wrapper, ms);
}

}//namespace
