#include "Event.h"

using std::mutex;
using std::lock_guard;
using std::unique_lock;

Event::Event()
	: _set(false)
{
}

void Event::set()
{
	lock_guard<mutex> myLock(_mutex);
	_set = true;
	_cond.notify_all();
}

bool Event::wait() const
{
	unique_lock<mutex> myLock(_mutex);
	while (!_set)
		_cond.wait(myLock);
	return _set;
}
