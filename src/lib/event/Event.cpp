/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "Event.h"

#include <chrono>
using std::mutex;
using std::unique_lock;
using namespace std::chrono;

Event::Event()
	: _shutdown(false)
{
}

void Event::signal()
{
	_cond.notify_all();
}

void Event::signalOne()
{
	_cond.notify_one();
}

// TODO: instead of a blanket shutdown, what about a std::atomic<int> that we check to see
// before we call wait?
// e.g, signal(100)
void Event::shutdown()
{
	unique_lock<mutex> myLock(_mutex);
	_shutdown = true;
	signal();
}

bool Event::wait(unsigned millis) const
{
	unique_lock<mutex> myLock(_mutex);
	if (_shutdown)
		return true;
	std::cv_status res = _cond.wait_for(myLock, milliseconds(millis));
	return res != std::cv_status::timeout; // false would be if we hit our timeout
}
//*/
