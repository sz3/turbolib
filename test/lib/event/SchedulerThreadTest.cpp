/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "SchedulerThread.h"

#include "Event.h"
#include "time/Timer.h"
#include "serialize/StringUtil.h"
#include <vector>
using std::bind;
using std::vector;

TEST_CASE( "SchedulerThreadTest/testSchedule", "default" )
{
	SchedulerThread scheduler;

	Timer time;
	vector<unsigned> results;
	vector<unsigned> timing;
	Event midway;
	Event finished;

	scheduler.schedule( bind(&Event::signal, &finished), 150 );
	scheduler.schedule( [&] () { results.push_back(5); }, 5 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 5 );
	scheduler.schedule( [&] () { results.push_back(50); }, 50 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 50 );
	scheduler.schedule( bind(&Event::signal, &midway), 70 );
	scheduler.schedule( [&] () { results.push_back(100); }, 100 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 100 );

	assertTrue( midway.wait(700) );

	scheduler.schedule( [&] () { results.push_back(100); }, 30 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 30 );
	scheduler.schedule( [&] () { results.push_back(130); }, 60 );
	scheduler.schedule( [&] () { timing.push_back(time.millis()); }, 60 );

	assertTrue( finished.wait(1000) );
	assertEquals( "5 50 100 100 130", StringUtil::join(results) );

	assertEquals(5, timing.size());
	assertInRange(4, 6, timing[0]);
	assertInRange(47, 53, timing[1]);
	assertInRange(97, 103, timing[2]);
	assertInRange(97, 103, timing[3]);
	assertInRange(127, 133, timing[4]);
}

TEST_CASE( "SchedulerThreadTest/testPeriodic", "default" )
{
	SchedulerThread scheduler;

	Timer time;
	vector<unsigned> results;
	vector<unsigned> timing;
	Event finished;

	scheduler.schedule( bind(&Event::signal, &finished), 100 );

	scheduler.schedulePeriodic( [&] () { results.push_back(30); }, 30 );
	scheduler.schedulePeriodic( [&] () { timing.push_back(time.millis()); }, 30 );

	assertTrue( finished.wait(1000) );
	assertEquals( "30 30 30", StringUtil::join(results) );

	assertEquals(3, timing.size());
	assertInRange(27, 33, timing[0]);
	assertInRange(54, 66, timing[1]);
	assertInRange(81, 99, timing[2]);
}

