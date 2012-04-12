#pragma once

#include <condition_variable>
#include <mutex>

class Event
{
public:
	Event();

	void set();
	bool wait(unsigned millis = ~0) const;

protected:
	mutable std::mutex _mutex;
	mutable std::condition_variable _cond;
};

