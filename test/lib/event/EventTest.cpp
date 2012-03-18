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

void testEventFun(Event& event, int num)
{
	std::time_t startTime(std::time(NULL));
	std::cerr << num << " sup dudes! " << std::ctime(&startTime) << std::endl;

	Timer t;

	event.wait();

	std::cerr << "time taken: " << t.duration().count() << "," << t.micros() << "," << t.millis() << std::endl;
}

TEST_CASE( "EventTest/testDefault", "default" )
{
	Event event;

	std::thread b1( bind(testEventFun, ref(event), 1) );
	std::thread b2( bind(testEventFun, ref(event), 2) );
	std::thread b3( bind(testEventFun, ref(event), 3) );

	::sleep(1);
	std::cerr << "hi." << std::endl;
	event.set();

	b1.join();
	b2.join();
	b3.join();

	REQUIRE( true );
}

