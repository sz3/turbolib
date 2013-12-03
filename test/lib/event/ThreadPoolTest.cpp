#include "unittest.h"

#include "ThreadPool.h"

#include "Event.h"
#include "serialize/StringUtil.h"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <vector>
using std::lock_guard;
using std::mutex;

TEST_CASE( "ThreadPoolTest/testDefault", "[unit]" )
{
	ThreadPool threads(4);
	assertTrue( threads.start() );

	mutex myMutex;
	std::vector<unsigned> results;
	for (unsigned i = 0; i < 10; ++i)
		threads.execute( [&results, &myMutex, i] () { lock_guard<mutex> myLock(myMutex); results.push_back(i); } );

	Event event;
	threads.execute( [&event] () { event.shutdown(); } );
	assertTrue( event.wait(1000) );

	while (results.size() < 10)
		std::cout << threads.queued() << std::endl;

	std::sort(results.begin(), results.end());
	assertEquals( "0 1 2 3 4 5 6 7 8 9", StringUtil::stlJoin(results) );
}

