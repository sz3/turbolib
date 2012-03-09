#include "catch.hpp"

#include "common/Event.h"
#include <functional>
#include <iostream>
#include <thread>
#include <unistd.h>

using std::bind;
using std::ref;

void testEventFun(Event& event)
{
	std::cout << "sup dudes!" << std::endl;
	std::cout << "wait returned: " << event.wait() << std::endl;
	std::cout << "I'm freeee!" << std::endl;
}

TEST_CASE( "ThreadTest/testDefault", "default" )
{
	Event event;

	std::thread background( bind(testEventFun, ref(event)) );

	::sleep(1);
	std::cout << "hi." << std::endl;
	event.set();

	background.join();

	REQUIRE( event.wait() );
}

