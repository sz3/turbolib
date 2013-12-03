#pragma once

#include "Event.h"
#include <atomic>

class CounterEvent
{
public:
	CounterEvent(unsigned count);

	void signal();
	bool wait(unsigned millis = ~0);

protected:
	std::atomic<unsigned> _seen;
	unsigned _desired;
	Event _event;
};
