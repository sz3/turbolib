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

TEST_CASE( "StringUtilTest/testStlJoinPrimitive", "description" )
{
	std::vector<int> numbers;
	for (int i = 0; i < 10; ++i)
		numbers.push_back(i);

	string toStr = StringUtil::stlJoin(numbers);
	assertEquals( "0 1 2 3 4 5 6 7 8 9", toStr );
}

TEST_CASE( "StringUtilTest/testStlJoin", "description" )
{
	std::deque<string> words = {"go", "pack", "go"};

	string toStr = StringUtil::stlJoin(words);
	assertEquals( "go pack go", toStr );
}

TEST_CASE( "StringUtilTest/testStlJoinCustomDelimiter", "description" )
{
	std::set<string> words = {"Brady", "Manning"};

	string toStr = StringUtil::stlJoin(words, '<');
	assertEquals( "Brady<Manning", toStr );
}

TEST_CASE( "StringUtilTest/testStlJoinMap", "description" )
{
	map<int,int> grid;
	for (int i = 0; i < 5; ++i)
		grid[i] = i*10;

	string toStr = StringUtil::stlJoin(grid);
	assertEquals( "0=0 1=10 2=20 3=30 4=40", toStr );
}

