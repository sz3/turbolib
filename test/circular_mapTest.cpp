/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "circular_map.h"
#include <string>
using turbo::circular_map;
using namespace std;

TEST_CASE( "circular_mapTest/testLoop", "[unit]" )
{
	circular_map<int, string> circle;
	circle[0] = "hi";
	circle.insert( std::pair<int,string>(1, "bye") );
	circle[2] = "foo";
	circle[4] = "bar";

	{
		string result;

		circular_map<int,string>::circular_iterator it = circle.find(2);
		for (; it != circle.end(); ++it)
			result += it->second;

		assertEquals( "foobarhibye", result );
	}

	{
		string result;

		circular_map<int,string>::circular_iterator it = circle.lower_bound(3);
		for (; it != circle.end(); ++it)
			result += it->second;

		assertEquals( "barhibyefoo", result );
	}

	{
		string result;

		circular_map<int,string>::circular_iterator it = circle.upper_bound(3);
		for (; it != circle.end(); ++it)
			result += it->second;

		assertEquals( "barhibyefoo", result );
	}
}

TEST_CASE( "circular_mapTest/testOvershoot", "[unit]" )
{
	circular_map<int, string> circle;
	circle[0] = "hi";
	circle[2] = "foo";
	circle[4] = "bar";

	{
		string result;

		circular_map<int,string>::circular_iterator it = circle.lower_bound(7);
		for (; it != circle.end(); ++it)
			result += it->second;

		assertEquals( "hifoobar", result );
	}

	{
		string result;

		circular_map<int,string>::circular_iterator it = circle.upper_bound(7);
		for (; it != circle.end(); ++it)
			result += it->second;

		assertEquals( "hifoobar", result );
	}
}
