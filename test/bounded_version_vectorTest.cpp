/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "bounded_version_vector.h"
#include <iostream>
#include <sstream>
#include <string>
using std::string;
using vector_clock::bounded_version_vector;

namespace {
	std::ostream& operator<<(std::ostream& outstream, const bounded_version_vector<string>::clock& clock)
	{
		outstream << clock.key << ":" << clock.count;
		return outstream;
	}

	std::ostream& operator<<(std::ostream& outstream, const bounded_version_vector<string,4>::clock& clock)
	{
		outstream << clock.key << ":" << clock.count;
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

TEST_CASE( "bounded_version_vectorTest/testIncrement", "[unit]" )
{
	bounded_version_vector<string> version;
	version.increment("foo");
	version.increment("bar");
	assertEquals( "bar:1 foo:1", join(version.clocks()) );

	version.increment("foo");
	assertEquals( "foo:2 bar:1", join(version.clocks()) );

	version.increment("oof");
	assertEquals( "oof:1 foo:2 bar:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testEmpty", "[unit]" )
{
	bounded_version_vector<string> version;
	assertTrue( version.empty() );

	version.increment("foo");
	assertFalse( version.empty() );

	version.increment("bar");
	assertFalse( version.empty() );

	version.clear();
	assertTrue( version.empty() );
}

TEST_CASE( "bounded_version_vectorTest/testLimits", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;
	version.increment("get out");
	version.increment("get out");
	version.increment("foo");
	version.increment("bar");
	version.increment("oof");
	version.increment("rab");

	assertEquals( "rab:1 oof:1 bar:1 foo:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testConstructors", "[unit]" )
{
	bounded_version_vector<string> version;
	assertEquals( "", join(version.clocks()) );

	version.increment("foo");
	version.increment("bar");

	bounded_version_vector<string> other(version.clocks());
	assertEquals( "bar:1 foo:1", join(other.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testCompare", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;
	VectorClock empty;

	VectorClock one;
	one.increment("foo");

	VectorClock two;
	two.increment("foo");
	two.increment("bar");

	VectorClock alternate;
	alternate.increment("foo");
	alternate.increment("oof");

	assertEquals( VectorClock::EQUAL, empty.compare(empty) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(one) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(two) );
	assertEquals( VectorClock::LESS_THAN, empty.compare(alternate) );

	assertEquals( VectorClock::GREATER_THAN, one.compare(empty) );
	assertEquals( VectorClock::EQUAL, one.compare(one) );
	assertEquals( VectorClock::LESS_THAN, one.compare(two) );
	assertEquals( VectorClock::LESS_THAN, one.compare(alternate) );

	assertEquals( VectorClock::GREATER_THAN, two.compare(empty) );
	assertEquals( VectorClock::GREATER_THAN, two.compare(one) );
	assertEquals( VectorClock::EQUAL, two.compare(two) );
	assertEquals( VectorClock::CONFLICT, two.compare(alternate) );

	assertEquals( VectorClock::GREATER_THAN, alternate.compare(empty) );
	assertEquals( VectorClock::GREATER_THAN, alternate.compare(one) );
	assertEquals( VectorClock::CONFLICT, alternate.compare(two) );
	assertEquals( VectorClock::EQUAL, alternate.compare(alternate) );

	assertTrue( empty < one );
	assertTrue( one < alternate );
	assertFalse( alternate < one );
	assertFalse( alternate < two );
	assertFalse( two < alternate );
}

TEST_CASE( "bounded_version_vectorTest/testCompare.More", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;

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

TEST_CASE( "bounded_version_vectorTest/testCompare.Limit", "[unit]" )
{
	// since we keep a finite amount of clocks, there's special logic to "discard" old ones
	//   if the conditions are right
	// we'll get a GREATER_THAN (or LESS_THAN) where a standard comparison would return CONFLICT

	using VectorClock = bounded_version_vector<string, 4>;

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

TEST_CASE( "bounded_version_vectorTest/testMerge", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;

	VectorClock version;

	VectorClock one;
	one.increment("foo");

	version.merge(one);
	assertEquals( "foo:1", join(version.clocks()) );

	VectorClock two;
	two.increment("foo");
	two.increment("bar");

	version.merge(two);
	assertEquals( "bar:1 foo:1", join(version.clocks()) );

	one.increment("foo");
	version.merge(one);

	// no definive order, but the algorithm says to alternate between conflicted clocks,
	// beginning with self. So bar stays first.
	assertEquals( "bar:1 foo:2", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.NewConflict", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;

	VectorClock version;
	version.increment("foo");

	VectorClock two;
	two.increment("bar");

	version.merge(two);
	assertEquals( "foo:1 bar:1", join(version.clocks()) );

	VectorClock three;
	three.increment("foo");
	three.increment("oof");

	version.merge(three);
	assertEquals( "oof:1 foo:1 bar:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.OldConflict.CountGt", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;

	VectorClock version;
	version.increment("foo");
	version.increment("bar");
	version.increment("foo");

	VectorClock three;
	three.increment("oof");
	three.increment("foo");

	version.merge(three);
	assertEquals( "foo:2 oof:1 bar:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.OldConflict.CountEq", "[unit]" )
{
	using VectorClock = bounded_version_vector<string>;

	VectorClock version;
	version.increment("foo");
	version.increment("bar");
	version.increment("foo");

	VectorClock three;
	three.increment("oof");
	three.increment("foo");
	three.increment("foo");

	version.merge(three);
	assertEquals( "foo:2 oof:1 bar:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.Limit", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;
	version.increment("1");
	version.increment("2");
	version.increment("3");
	version.increment("4");

	VectorClock other;
	other.increment("3");
	other.increment("4");
	other.increment("5");
	other.increment("6");

	version.merge(other);
	assertEquals( "6:1 5:1 4:1 3:1", join(version.clocks()) );

	other.clear();
	other.increment("6");
	other.increment("3");
	other.increment("3");
	other.increment("7");
	other.increment("8");
	version.merge(other);
	assertEquals( "8:1 7:1 3:2 6:1", join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.CompleteConflict.VerifyCompareGreater", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1");
	one.increment("2");
	one.increment("3");
	one.increment("4");
	version.merge(one);
	assertEquals( "4:1 3:1 2:1 1:1", join(version.clocks()) );

	VectorClock two;
	two.increment("5");
	two.increment("6");
	two.increment("7");
	two.increment("8");

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1 8:1 3:1 7:1", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.PartialConflict", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1");
	one.increment("2");
	one.increment("3");
	one.increment("4");
	version.merge(one);
	assertEquals( "4:1 3:1 2:1 1:1", join(version.clocks()) );

	VectorClock two;
	two.increment("1");
	two.increment("6");
	two.increment("7");
	two.increment("8");

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1 8:1 3:1 7:1", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.CompleteConflict.VersionGreater", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1");
	one.increment("2");
	one.increment("3");
	one.increment("4");
	version.merge(one);
	assertEquals( "4:1 3:1 2:1 1:1", join(version.clocks()) );

	VectorClock two;
	two.increment("6");
	two.increment("1");
	two.increment("1");
	two.increment("7");
	two.increment("8");

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1 8:1 3:1 7:1", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}

TEST_CASE( "bounded_version_vectorTest/testMerge.WeirdConflict", "[unit]" )
{
	using VectorClock = bounded_version_vector<string, 4>;

	VectorClock version;

	VectorClock one;
	one.increment("1");
	one.increment("2");
	one.increment("3");
	one.increment("4");
	version.merge(one);
	assertEquals( "4:1 3:1 2:1 1:1", join(version.clocks()) );

	VectorClock two;
	two.increment("2");
	two.increment("1");
	two.increment("7");
	two.increment("8");

	version.merge(two);
	// no point of reference, interlace
	assertEquals( "4:1 8:1 3:1 7:1", join(version.clocks()) );

	assertEquals( VectorClock::GREATER_THAN, version.compare(one) );
	assertEquals( VectorClock::GREATER_THAN, version.compare(two) );
}
