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
	assertEquals( "5 50 100 100 130", StringUtil::stlJoin(results) );

	assertEquals(5, timing.size());
	assertInRange(4, 5, timing[0]);
	assertInRange(48, 50, timing[1]);
	assertInRange(98, 100, timing[2]);
	assertInRange(98, 100, timing[3]);
	assertInRange(128, 130, timing[4]);
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
	assertEquals( "30 30 30", StringUtil::stlJoin(results) );

	assertEquals(3, timing.size());
	assertInRange(28, 30, timing[0]);
	assertInRange(56, 60, timing[1]);
	assertInRange(84, 90, timing[2]);
}

