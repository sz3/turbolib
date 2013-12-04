#include "Event.h"

#include <chrono>
using std::mutex;
using std::lock_guard;
using std::unique_lock;
using namespace std::chrono;

Event::Event()
	: _shutdown(false)
{
}

void Event::signal()
{
	lock_guard<mutex> myLock(_mutex);
	_cond.notify_all();
}

void Event::signalOne()
{
	lock_guard<mutex> myLock(_mutex);
	_cond.notify_one();
}

// TODO: instead of a blanket shutdown, what about a std::atomic<int> that we check to see
// before we call wait?
// e.g, signal(100)
void Event::shutdown()
{
	lock_guard<mutex> myLock(_mutex);
	_cond.notify_all();
	_shutdown = true;
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
