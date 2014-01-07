/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
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
