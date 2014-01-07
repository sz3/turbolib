#include "unittest.h"

#include "MultiThreadedExecutor.h"

#include "Event.h"
#include "time/Timer.h"
#include "serialize/StringUtil.h"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <vector>
using std::lock_guard;
using std::mutex;

TEST_CASE( "MultiThreadedExecutorTest/testDefault", "[unit]" )
{
	MultiThreadedExecutor threads(5);
	assertTrue( threads.start() );

	Timer t;
	mutex myMutex;
	std::vector<unsigned> results;
	std::vector<unsigned> times;
	for (unsigned i = 0; i < 10; ++i)
		threads.execute( [&, i] () { lock_guard<mutex> myLock(myMutex); results.push_back(i); times.push_back(t.micros()); } );

	Event event;
	threads.execute( [&event] () { event.shutdown(); } );
	assertTrue( event.wait(1000) );

	while (results.size() < 10)
		std::cout << threads.queued() << std::endl;

	std::sort(results.begin(), results.end());
	assertEquals( "0 1 2 3 4 5 6 7 8 9", StringUtil::stlJoin(results) );

	std::cout << "times: " << StringUtil::stlJoin(times) << std::endl;
}

