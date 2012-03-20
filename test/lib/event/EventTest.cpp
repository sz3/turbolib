#include "catch.hpp"

#include "Event.h"
#include "Timer.h"
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
	EventScience(const Event& event, const Timer& startup, int num)
		: _timer(NULL)
		, _thread( bind(&EventScience::testEventFun, this, ref(event), ref(startup), num) )
	{
	}

	void testEventFun(const Event& event, const Timer& startup, int num)
	{
		_results << num << " time taken to start thread: " << startup.micros() << "us, " << startup.millis() << "ms" << std::endl;

		std::time_t startTime(std::time(NULL));
		std::cerr << num << " sup dudes! " << std::ctime(&startTime);

		event.wait();

		if (_timer != NULL)
			_results << num << " time taken for event to propagate: " << _timer->micros() << "us, " << _timer->millis() << "ms" << std::endl;
	}

	void setTimer(const Timer* timer)
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
	const Timer* _timer;
	std::thread  _thread;
	std::stringstream  _results;
};

TEST_CASE( "EventTest/testDefault", "default" )
{
	Event event;

	Timer startup;
	EventScience science1(event, startup, 1);
	EventScience science2(event, startup, 2);
	EventScience science3(event, startup, 3);

	::sleep(1);
	std::cerr << "hi." << std::endl;
	Timer t;

	science1.setTimer(&t);
	science2.setTimer(&t);
	science3.setTimer(&t);
	event.set();

	science1.stop_and_wait();
	science2.stop_and_wait();
	science3.stop_and_wait();

	std::cout << science1.results() << std::endl;
	std::cout << science2.results() << std::endl;
	std::cout << science3.results() << std::endl;

	REQUIRE( true );
}

