#include "Event.h"

using std::mutex;
using std::lock_guard;
using std::unique_lock;

Event::Event()
{
}

void Event::set()
{
	lock_guard<mutex> myLock(_mutex);
	_cond.notify_all();
}

bool Event::wait() const
{
	unique_lock<mutex> myLock(_mutex);
	_cond.wait(myLock);
	return true; // false would be if we hit our timeout
}
