/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "catch.hpp"

#include "Event.h"
#include "time/stopwatch.h"
#include <ctime>
#include <functional>
#include <iostream>
#include <thread>
#include <unistd.h>

using std::bind;
using std::ref;

class EventScience
{
public:
	EventScience(const Event& event, const stopwatch& startup, int num)
		: _timer(NULL)
		, _thread( bind(&EventScience::testEventFun, this, ref(event), ref(startup), num) )
	{
	}

	void testEventFun(const Event& event, const stopwatch& startup, int num)
	{
		_results << num << " time taken to start thread: " << startup.micros() << "us, " << startup.millis() << "ms" << std::endl;

		std::time_t startTime(std::time(NULL));
		std::cerr << num << " sup dudes! " << std::ctime(&startTime);

		event.wait();

		if (_timer != NULL)
			_results << num << " time taken for event to propagate: " << _timer->micros() << "us, " << _timer->millis() << "ms" << std::endl;
	}

	void setStopwatch(const stopwatch* timer)
	{
		_timer = timer;
	}

	void stop_and_wait()
	{
		_thread.join();
	}

	std::string results() const
	{
		return _results.str();
	}

protected:
	const stopwatch* _timer;
	std::stringstream  _results;
	std::thread  _thread;
};

TEST_CASE( "EventTest/testDefault", "default" )
{
	Event event;

	stopwatch startup;
	EventScience science1(event, startup, 1);
	EventScience science2(event, startup, 2);
	EventScience science3(event, startup, 3);

	::sleep(1);
	std::cerr << "hi." << std::endl;
	stopwatch t;

	science1.setStopwatch(&t);
	science2.setStopwatch(&t);
	science3.setStopwatch(&t);
	event.signal();

	science1.stop_and_wait();
	science2.stop_and_wait();
	science3.stop_and_wait();

	std::cout << science1.results() << std::endl;
	std::cout << science2.results() << std::endl;
	std::cout << science3.results() << std::endl;

	REQUIRE( true );
}

