#include "catch.hpp"

#include "Event.h"
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

	event.wait();

	std::time_t endTime(std::time(NULL));
	std::cerr << num << " I'm freeee! " << std::ctime(&endTime) << std::endl;
}

TEST_CASE( "EventTest/testDefault", "default" )
{
	Event event;

	std::thread b1( bind(testEventFun, ref(event), 1) );
	std::thread b2( bind(testEventFun, ref(event), 2) );

	::sleep(1);
	std::cerr << "hi." << std::endl;
	event.set();

	b1.join();
	b2.join();

	REQUIRE( true );
}

