#pragma once

#include <condition_variable>
#include <mutex>

class Event
{
public:
	Event();

	void set();
	bool wait() const;

protected:
	mutable std::mutex _mutex;
	mutable std::condition_variable _cond;
	bool _set;
};

