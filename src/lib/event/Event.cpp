#include "Event.h"

#include <chrono>
using std::mutex;
using std::lock_guard;
using std::unique_lock;
using namespace std::chrono;

Event::Event()
{
}

void Event::signal()
{
	lock_guard<mutex> myLock(_mutex);
	_cond.notify_all();
}

bool Event::wait(unsigned millis) const
{
	unique_lock<mutex> myLock(_mutex);
	std::cv_status res = _cond.wait_for(myLock, milliseconds(millis));
	return res != std::cv_status::timeout; // false would be if we hit our timeout
}
//*/
