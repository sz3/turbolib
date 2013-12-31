#include "CounterEvent.h"

CounterEvent::CounterEvent(unsigned count)
	: _seen(0)
	, _desired(count)
{
}

void CounterEvent::signal()
{
	_seen++;
	if (_seen == _desired)
		_event.shutdown();
}

bool CounterEvent::wait(unsigned millis)
{
	return _event.wait(millis);
}
