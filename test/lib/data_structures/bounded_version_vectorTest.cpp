/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "bounded_version_vector.h"
#include <iostream>
#include <string>
#include "serialize/StringUtil.h"
using std::string;

namespace {
	std::ostream& operator<<(std::ostream& outstream, const bounded_version_vector<string>::clock& clock)
	{
		outstream << clock.key << ":" << clock.count;
		return outstream;
	}
}

TEST_CASE( "bounded_version_vectorTest/testIncrement", "[unit]" )
{
	bounded_version_vector<string> version;
	version.increment("foo");
	version.increment("bar");
	assertEquals( "bar:1 foo:1", StringUtil::join(version.clocks()) );

	version.increment("foo");
	assertEquals( "foo:2 bar:1", StringUtil::join(version.clocks()) );

	version.increment("oof");
	assertEquals( "oof:1 foo:2 bar:1", StringUtil::join(version.clocks()) );
}

TEST_CASE( "bounded_version_vectorTest/testConstructors", "[unit]" )
{
	bounded_version_vector<string> version;
	assertEquals( "", StringUtil::join(version.clocks()) );

	version.increment("foo");
	version.increment("bar");

	bounded_version_vector<string> other(version.clocks());
	assertEquals( "bar:1 foo:1", StringUtil::join(other.clocks()) );
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
}
