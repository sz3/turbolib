/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "bounded_time_vector.h"
#include <iostream>
#include <sstream>
#include <string>
using std::string;
using vector_clock::bounded_time_vector;

namespace {
	std::ostream& operator<<(std::ostream& outstream, const bounded_time_vector<string>::clock& clock)
	{
		outstream << clock.key << ":" << clock.time << "." << clock.count;
		return outstream;
	}

	std::ostream& operator<<(std::ostream& outstream, const bounded_time_vector<string,4>::clock& clock)
	{
		outstream << clock.key << ":" << clock.time << "." << clock.count;
		return outstream;
	}

	template <typename Container>
	string join(const Container& clocks)
	{
		std::stringstream ss;
		auto it = clocks.begin();
		if (it != clocks.end())
			ss << *it++;
		for (; it != clocks.end(); ++it)
			ss << " " << *it;
		return ss.str();
	}
}

TEST_CASE( "bounded_time_vectorTest/testIncrement", "[unit]" )
{
	bounded_time_vector<string> version;
	version.increment("foo", 10);
	version.increment("bar", 20);
	assertEquals( "bar:20.0 foo:10.0", join(version.clocks()) );

	version.increment("foo", 11);
	assertEquals( "foo:11.0 bar:20.0", join(version.clocks()) );

	version.increment("foo", 5);
	assertEquals( "foo:11.1 bar:20.0", join(version.clocks()) );

	version.increment("foo", 11);
	assertEquals( "foo:11.2 bar:20.0", join(version.clocks()) );

	version.increment("foo", 25);
	assertEquals( "foo:25.0 bar:20.0", join(version.clocks()) );

	version.increment("oof", 20);
	assertEquals( "oof:20.0 foo:25.0 bar:20.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testEmpty", "[unit]" )
{
	bounded_time_vector<string> version;
	assertTrue( version.empty() );

	version.increment("foo");
	assertFalse( version.empty() );

	version.increment("bar");
	assertFalse( version.empty() );

	version.clear();
	assertTrue( version.empty() );
}

TEST_CASE( "bounded_time_vectorTest/testLimits", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;
	version.increment("get out");
	version.increment("get out");
	version.increment("foo", 1);
	version.increment("bar", 1);
	version.increment("oof", 1);
	version.increment("rab", 1);

	assertEquals( "rab:1.0 oof:1.0 bar:1.0 foo:1.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testConstructors", "[unit]" )
{
	bounded_time_vector<string> version;
	assertEquals( "", join(version.clocks()) );

	version.increment("foo", 1);
	version.increment("bar", 1);

	bounded_time_vector<string> other(version.clocks());
	assertEquals( "bar:1.0 foo:1.0", join(other.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testCompare", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;
	VectorClock empty;

	VectorClock one;
	one.increment("foo", 1);

	VectorClock two;
	two.increment("foo", 1);
	two.increment("bar", 1);

	VectorClock alternate;
	alternate.increment("foo", 1);
	alternate.increment("oof", 1);

	VectorClock another;
	another.increment("foo", 2);

	assertEquals( VectorClock::EQUAL, empty.compare(empty) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(one) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(two) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(alternate) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(another) );

	assertEquals( VectorClock::GREATER_THAN, one.compare(empty) );
	assertEquals( VectorClock::EQUAL, one.compare(one) );
	assertEquals( VectorClock::LESS_THAN, one.compare(two) );
	assertEquals( VectorClock::LESS_THAN, one.compare(alternate) );
	assertEquals( VectorClock::LESS_THAN, one.compare(another) );

	assertEquals( VectorClock::GREATER_THAN, two.compare(empty) );
	assertEquals( VectorClock::GREATER_THAN, two.compare(one) );
	assertEquals( VectorClock::EQUAL, two.compare(two) );
	assertEquals( VectorClock::CONFLICT, two.compare(alternate) );
	assertEquals( VectorClock::CONFLICT, two.compare(another) );

	assertEquals( VectorClock::GREATER_THAN, alternate.compare(empty) );
	assertEquals( VectorClock::GREATER_THAN, alternate.compare(one) );
	assertEquals( VectorClock::CONFLICT, alternate.compare(two) );
	assertEquals( VectorClock::EQUAL, alternate.compare(alternate) );
	assertEquals( VectorClock::CONFLICT, alternate.compare(another) );

	assertEquals( VectorClock::GREATER_THAN, another.compare(empty) );
	assertEquals( VectorClock::GREATER_THAN, another.compare(one) );
	assertEquals( VectorClock::CONFLICT, another.compare(two) );
	assertEquals( VectorClock::CONFLICT, another.compare(alternate) );
	assertEquals( VectorClock::EQUAL, another.compare(another) );

	assertTrue( empty < one );
	assertTrue( one < alternate );
	assertTrue( one < another );
	assertFalse( alternate < one );
	assertFalse( alternate < two );
	assertFalse( alternate < another );
	assertFalse( two < alternate );
	assertFalse( another < alternate );
}

TEST_CASE( "bounded_time_vectorTest/testCompare.More", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;

	VectorClock one;
	one.increment("bar");
	one.increment("foo");

	VectorClock two;
	two.increment("bar");
	two.increment("foo");
	two.increment("foo");

	VectorClock invertTwo;
	invertTwo.increment("bar");
	invertTwo.increment("foo");
	invertTwo.increment("bar");

	assertEquals( VectorClock::EQUAL, one.compare(one) );
	assertEquals( VectorClock::LESS_THAN, one.compare(two) );
	assertEquals( VectorClock::LESS_THAN, one.compare(invertTwo) );

	assertEquals( VectorClock::GREATER_THAN, two.compare(one) );
	assertEquals( VectorClock::EQUAL, two.compare(two) );
	assertEquals( VectorClock::CONFLICT, two.compare(invertTwo) );

	assertEquals( VectorClock::GREATER_THAN, invertTwo.compare(one) );
	assertEquals( VectorClock::CONFLICT, invertTwo.compare(two) );
	assertEquals( VectorClock::EQUAL, invertTwo.compare(invertTwo) );
}

TEST_CASE( "bounded_time_vectorTest/testCompare.Limit", "[unit]" )
{
	// since we keep a finite amount of clocks, there's special logic to "discard" old ones
	//   if the conditions are right
	// we'll get a GREATER_THAN (or LESS_THAN) where a standard comparison would return CONFLICT

	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock old;
	old.increment("busted");
	old.increment("old");
	old.increment("foo");
	old.increment("bar");

	VectorClock current;
	current.increment("foo");
	current.increment("bar");
	current.increment("hotness");
	current.increment("new");

	// implication here is that we had two updates, ("hotness", then "new"),
	// which bumped our record of "old" and "busted" out of the current version vector.

	assertEquals( VectorClock::GREATER_THAN, current.compare(old) );
	assertEquals( VectorClock::LESS_THAN, old.compare(current) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;

	VectorClock version;

	VectorClock one;
	one.increment("foo", 1);

	version.merge(one);
	assertEquals( "foo:1.0", join(version.clocks()) );

	VectorClock two;
	two.increment("foo", 1);
	two.increment("bar", 1);

	version.merge(two);
	assertEquals( "bar:1.0 foo:1.0", join(version.clocks()) );

	one.increment("foo", 1);
	version.merge(one);

	// no definive order, but the algorithm says to alternate between conflicted clocks,
	// beginning with self. So bar stays first.
	assertEquals( "bar:1.0 foo:1.1", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.NewConflict", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;

	VectorClock version;
	version.increment("foo", 1);

	VectorClock two;
	two.increment("bar", 1);

	version.merge(two);
	assertEquals( "foo:1.0 bar:1.0", join(version.clocks()) );

	VectorClock three;
	three.increment("foo", 1);
	three.increment("oof", 1);

	version.merge(three);
	assertEquals( "oof:1.0 foo:1.0 bar:1.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.OldConflict.CountGt", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;

	VectorClock version;
	version.increment("foo", 1);
	version.increment("bar", 1);
	version.increment("foo", 1);

	VectorClock three;
	three.increment("oof", 1);
	three.increment("foo", 1);

	version.merge(three);
	assertEquals( "foo:1.1 oof:1.0 bar:1.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.OldConflict.CountEq", "[unit]" )
{
	using VectorClock = bounded_time_vector<string>;

	VectorClock version;
	version.increment("foo", 1);
	version.increment("bar", 1);
	version.increment("foo", 1);

	VectorClock three;
	three.increment("oof", 1);
	three.increment("foo", 1);
	three.increment("foo", 1);

	version.merge(three);
	assertEquals( "foo:1.1 oof:1.0 bar:1.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.Limit", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;
	version.increment("1", 1);
	version.increment("2", 1);
	version.increment("3", 1);
	version.increment("4", 1);

	VectorClock other;
	other.increment("3", 1);
	other.increment("4", 1);
	other.increment("5", 1);
	other.increment("6", 1);

	version.merge(other);
	assertEquals( "6:1.0 5:1.0 4:1.0 3:1.0", join(version.clocks()) );

	other.clear();
	other.increment("6", 1);
	other.increment("3", 1);
	other.increment("3", 1);
	other.increment("7", 1);
	other.increment("8", 1);
	version.merge(other);
	assertEquals( "8:1.0 7:1.0 3:1.1 6:1.0", join(version.clocks()) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.CompleteConflict.VerifyCompareGreater", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1", 1);
	one.increment("2", 1);
	one.increment("3", 1);
	one.increment("4", 1);
	version.merge(one);
	assertEquals( "4:1.0 3:1.0 2:1.0 1:1.0", join(version.clocks()) );

	VectorClock two;
	two.increment("5", 1);
	two.increment("6", 1);
	two.increment("7", 1);
	two.increment("8", 1);

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1.0 8:1.0 3:1.0 7:1.0", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.PartialConflict", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1", 1);
	one.increment("2", 1);
	one.increment("3", 1);
	one.increment("4", 1);
	version.merge(one);
	assertEquals( "4:1.0 3:1.0 2:1.0 1:1.0", join(version.clocks()) );

	VectorClock two;
	two.increment("1", 1);
	two.increment("6", 1);
	two.increment("7", 1);
	two.increment("8", 1);

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1.0 8:1.0 3:1.0 7:1.0", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.CompleteConflict.VersionGreater", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1", 1);
	one.increment("2", 1);
	one.increment("3", 1);
	one.increment("4", 1);
	version.merge(one);
	assertEquals( "4:1.0 3:1.0 2:1.0 1:1.0", join(version.clocks()) );

	VectorClock two;
	two.increment("6", 1);
	two.increment("1", 1);
	two.increment("1", 1);
	two.increment("7", 1);
	two.increment("8", 1);

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1.0 8:1.0 3:1.0 7:1.0", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_time_vectorTest/testMerge.WeirdConflict", "[unit]" )
{
	using VectorClock = bounded_time_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1", 1);
	one.increment("2", 1);
	one.increment("3", 1);
	one.increment("4", 1);
	version.merge(one);
	assertEquals( "4:1.0 3:1.0 2:1.0 1:1.0", join(version.clocks()) );

	VectorClock two;
	two.increment("2", 1);
	two.increment("1", 1);
	two.increment("7", 1);
	two.increment("8", 1);

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1.0 8:1.0 3:1.0 7:1.0", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}
