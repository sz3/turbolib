/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StringUtil.h"
#include "str_join.h"
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

TEST_CASE( "StringUtilTest/testSplit", "[unit]" )
{
	std::vector<string> split = StringUtil::split("0=0|1=10|2=20|3=30|4=40", '|');
	assertEquals( "0=0 1=10 2=20 3=30 4=40", turbo::str::join(split) );
	assertEquals( 5, split.size() );
}

TEST_CASE( "StringUtilTest/testSplitOne", "[unit]" )
{
	std::vector<string> split = StringUtil::split("hello", ' ');
	assertEquals( "hello", turbo::str::join(split) );
	assertEquals( 1, split.size() );
}

TEST_CASE( "StringUtilTest/testSplitNone", "[unit]" )
{
	std::vector<string> split = StringUtil::split("", ' ');
	assertEquals( 0, split.size() );
}

TEST_CASE( "StringUtilTest/testSplitEmpty", "[unit]" )
{
	string toSplit = " blah  blah blah ";
	std::vector<string> keepEmpty = StringUtil::split(toSplit, ' ');
	assertEquals( toSplit, turbo::str::join(keepEmpty) );
	assertEquals( 6, keepEmpty.size() );

	std::vector<string> tossEmpty = StringUtil::split(toSplit, ' ', true);
	assertEquals( "blah blah blah", turbo::str::join(tossEmpty) );
	assertEquals( 3, tossEmpty.size() );
}

TEST_CASE( "StringUtilTest/testStr", "[unit]" )
{
	assertEquals( "foo", StringUtil::str("foo") );
	assertEquals( "5", StringUtil::str(5) );
	assertEquals( "1.2", StringUtil::str(1.2) );
	assertEquals( "true", StringUtil::str(true) );
}

TEST_CASE( "StringUtilTest/testHexStr", "[unit]" )
{
	assertEquals( "5", StringUtil::hexStr(5) );
	assertEquals( "f", StringUtil::hexStr(15) );
	assertEquals( "f00", StringUtil::hexStr(0xF00) );
	assertEquals( "1234567812345678", StringUtil::hexStr(0x1234567812345678ULL) );
}

