/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "deadline_thread.h"

#include "concurrent/monitor.h"
#include "serialize/str_join.h"
#include "time/stopwatch.h"
#include <vector>
using std::bind;
using std::vector;
using turbo::deadline_thread;
using turbo::monitor;
using turbo::stopwatch;

TEST_CASE( "deadline_threadTest/testSchedule", "default" )
{
	deadline_thread scheduler;

	stopwatch time;
	vector<unsigned> results;
	vector<unsigned> timing;
	monitor midway;
	monitor finished;

	scheduler.schedule( bind(&monitor::signal_all, &finished), 150 );
	scheduler.schedule( [&] () { results.push_back(5); }, 5 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 5 );
	scheduler.schedule( [&] () { results.push_back(50); }, 50 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 50 );
	scheduler.schedule( bind(&monitor::signal_all, &midway), 70 );
	scheduler.schedule( [&] () { results.push_back(100); }, 100 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 100 );

	assertTrue( midway.wait_for(700) );

	scheduler.schedule( [&] () { results.push_back(100); }, 30 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 30 );
	scheduler.schedule( [&] () { results.push_back(130); }, 60 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 60 );

	assertTrue( finished.wait_for(1000) );
	assertEquals( "5 50 100 100 130", turbo::str::join(results) );

	assertEquals(5, timing.size());
	assertInRange(4, timing[0], 6);
	assertInRange(47, timing[1], 53);
	assertInRange(97, timing[2], 103);
	assertInRange(97, timing[3], 103);
	assertInRange(127, timing[4], 133);
}

TEST_CASE( "deadline_threadTest/testPeriodic", "default" )
{
	deadline_thread scheduler;

	stopwatch time;
	vector<unsigned> results;
	vector<unsigned> timing;
	monitor finished;

	scheduler.schedule( bind(&monitor::signal_all, &finished), 100 );

	scheduler.schedule_repeat( [&] () { results.push_back(30); }, 30 );
	scheduler.schedule_repeat( [&] () { timing.push_back(time.millis()); }, 30 );

	assertTrue( finished.wait_for(1000) );
	assertEquals( "30 30 30", turbo::str::join(results) );

	assertEquals(3, timing.size());
	assertInRange(27, timing[0], 33);
	assertInRange(54, timing[1], 66);
	assertInRange(81, timing[2], 99);
}

