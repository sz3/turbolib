/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StringUtil.h"
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

TEST_CASE( "StringUtilTest/testJoinPrimitive", "description" )
{
	std::vector<int> numbers;
	for (int i = 0; i < 10; ++i)
		numbers.push_back(i);

	string toStr = StringUtil::join(numbers);
	assertEquals( "0 1 2 3 4 5 6 7 8 9", toStr );
}

TEST_CASE( "StringUtilTest/testJoin", "description" )
{
	std::deque<string> words = {"go", "pack", "go"};

	string toStr = StringUtil::join(words);
	assertEquals( "go pack go", toStr );
}

TEST_CASE( "StringUtilTest/testJoinCustomDelimiter", "description" )
{
	std::set<string> words = {"Brady", "Manning"};

	string toStr = StringUtil::join(words, '<');
	assertEquals( "Brady<Manning", toStr );
}

TEST_CASE( "StringUtilTest/testJoinMap", "description" )
{
	map<int,int> grid;
	for (int i = 0; i < 5; ++i)
		grid[i] = i*10;

	string toStr = StringUtil::join(grid);
	assertEquals( "0=0 1=10 2=20 3=30 4=40", toStr );
}

TEST_CASE( "StringUtilTest/testSplit", "[unit]" )
{
	std::vector<string> split = StringUtil::split("0=0|1=10|2=20|3=30|4=40", '|');
	assertEquals( "0=0 1=10 2=20 3=30 4=40", StringUtil::join(split) );
	assertEquals( 5, split.size() );
}

TEST_CASE( "StringUtilTest/testSplitOne", "[unit]" )
{
	std::vector<string> split = StringUtil::split("hello", ' ');
	assertEquals( "hello", StringUtil::join(split) );
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
	assertEquals( toSplit, StringUtil::join(keepEmpty) );
	assertEquals( 6, keepEmpty.size() );

	std::vector<string> tossEmpty = StringUtil::split(toSplit, ' ', true);
	assertEquals( "blah blah blah", StringUtil::join(tossEmpty) );
	assertEquals( 3, tossEmpty.size() );
}

