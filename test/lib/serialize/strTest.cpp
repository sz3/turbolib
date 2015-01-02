/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "serialize/str.h"
#include <deque>
#include <map>
#include <set>
#include <vector>
using std::map;
using std::string;

namespace {
	template <class T1, class T2>
	std::ostream& operator<<(std::ostream& outstream, const std::pair<T1,T2>& pear)
	{
		outstream << pear.first << "=" << pear.second;
		return outstream;
	}
}
#include "serialize/str_join.h"

TEST_CASE( "strTest/testSplit", "[unit]" )
{
	std::vector<string> split = turbo::str::split("0=0|1=10|2=20|3=30|4=40", '|');
	assertEquals( "0=0 1=10 2=20 3=30 4=40", turbo::str::join(split) );
	assertEquals( 5, split.size() );
}

TEST_CASE( "strTest/testSplitOne", "[unit]" )
{
	std::vector<string> split = turbo::str::split("hello", ' ');
	assertEquals( "hello", turbo::str::join(split) );
	assertEquals( 1, split.size() );
}

TEST_CASE( "strTest/testSplitNone", "[unit]" )
{
	std::vector<string> split = turbo::str::split("", ' ');
	assertEquals( 0, split.size() );
}

TEST_CASE( "strTest/testSplitEmpty", "[unit]" )
{
	string toSplit = " blah  blah blah ";
	std::vector<string> keepEmpty = turbo::str::split(toSplit, ' ');
	assertEquals( toSplit, turbo::str::join(keepEmpty) );
	assertEquals( 6, keepEmpty.size() );

	std::vector<string> tossEmpty = turbo::str::split(toSplit, ' ', true);
	assertEquals( "blah blah blah", turbo::str::join(tossEmpty) );
	assertEquals( 3, tossEmpty.size() );
}

TEST_CASE( "strTest/testStr", "[unit]" )
{
	assertEquals( "foo", turbo::str::str("foo") );
	assertEquals( "5", turbo::str::str(5) );
	assertEquals( "1.2", turbo::str::str(1.2) );
	assertEquals( "true", turbo::str::str(true) );
}

TEST_CASE( "strTest/testHexStr", "[unit]" )
{
	assertEquals( "5", turbo::str::hexStr(5) );
	assertEquals( "f", turbo::str::hexStr(15) );
	assertEquals( "f00", turbo::str::hexStr(0xF00) );
	assertEquals( "1234567812345678", turbo::str::hexStr(0x1234567812345678ULL) );
}

