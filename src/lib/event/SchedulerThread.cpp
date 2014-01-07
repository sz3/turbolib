/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "SchedulerThread.h"

#include <iostream>
using namespace std::chrono;
using std::lock_guard;
using std::mutex;

namespace {
	system_clock::time_point now()
	{
		return system_clock::now();
	}

	long long millisFromNow(const system_clock::time_point& then)
	{
		return duration_cast<milliseconds>(then - now()).count();
	}
}

SchedulerThread::SchedulerThread()
	: _shutdown(false)
	, _thread(std::bind(&SchedulerThread::run, this))
{
}

SchedulerThread::~SchedulerThread()
{
	shutdown();
}

void SchedulerThread::shutdown()
{
	if (_shutdown)
		return;
	_shutdown = true;
	_notifyWork.signal();
	_thread.join();
}

void SchedulerThread::run()
{
	while (!_shutdown)
	{
		waitForWork();
		while(doWork());
	}
}

bool SchedulerThread::doWork()
{
	if (_shutdown)
		return false;

	std::function<void()> work;
	{
		lock_guard<mutex> myLock(_mutex);
		if (_funs.empty())
			return false;

		const Funct& top = _funs.top();
		if (millisFromNow(top.time) > 0) // would like to use operator<, but apparently it doesn't work?
			return false;

		work = top.fun;
		_funs.pop();
	}
	work();
	return true;
}

void SchedulerThread::waitForWork()
{
	unsigned interval = ~0;
	{
		lock_guard<mutex> myLock(_mutex);
		if (!_funs.empty())
		{
			const Funct& top = _funs.top();
			long long duration = millisFromNow(top.time);
			interval = (duration < 0)? 0 : duration;
		}
	}
	// would be good to have a _logger here for a test to sanity check that we're sleeping properly, not spin-waiting.
	//std::cout << "waiting " << interval << std::endl;
	_notifyWork.wait(interval);
}

void SchedulerThread::schedule(const std::function<void()>& fun, unsigned millis)
{
	system_clock::time_point when = now() + milliseconds(millis);
	{
		lock_guard<mutex> myLock(_mutex);
		_funs.push( {when, fun} );
	}
	_notifyWork.signal();
}

void SchedulerThread::schedulePeriodic(const std::function<void()>& fun, unsigned millis)
{
	std::function<void()> wrapper( [=] () { fun(); this->schedulePeriodic(fun, millis); } );
	schedule(wrapper, millis);
}
