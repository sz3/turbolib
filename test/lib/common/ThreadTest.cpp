#include "catch.hpp"

#include "common/Event.h"
#include <ctime>
#include <functional>
#include <iostream>
#include <thread>
#include <unistd.h>

using std::bind;
using std::ref;

void testEventFun(Event& event)
{
	std::time_t startTime(std::time(NULL));
	std::cerr << "sup dudes! " << std::ctime(&startTime) << std::endl;

	event.wait();

	std::time_t endTime(std::time(NULL));
	std::cerr << "I'm freeee! " << std::ctime(&endTime) << std::endl;
}

TEST_CASE( "ThreadTest/testDefault", "default" )
{
	Event event;

	std::thread background( bind(testEventFun, ref(event)) );

	::sleep(1);
	std::cerr << "hi." << std::endl;
	event.set();

	background.join();

	REQUIRE( true );
}

