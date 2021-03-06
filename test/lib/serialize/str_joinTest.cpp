/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

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

TEST_CASE( "str_joinTest/testJoinPrimitive", "description" )
{
	std::vector<int> numbers;
	for (int i = 0; i < 10; ++i)
		numbers.push_back(i);

	string toStr = turbo::str::join(numbers);
	assertEquals( "0 1 2 3 4 5 6 7 8 9", toStr );
}

TEST_CASE( "str_joinTest/testJoin", "description" )
{
	std::deque<string> words = {"go", "pack", "go"};

	string toStr = turbo::str::join(words);
	assertEquals( "go pack go", toStr );
}

TEST_CASE( "str_joinTest/testJoinCustomDelimiter", "description" )
{
	std::set<string> words = {"Brady", "Manning"};

	string toStr = turbo::str::join(words, '<');
	assertEquals( "Brady<Manning", toStr );
}

TEST_CASE( "str_joinTest/testJoinMap", "description" )
{
	map<int,int> grid;
	for (int i = 0; i < 5; ++i)
		grid[i] = i*10;

	string toStr = turbo::str::join(grid);
	assertEquals( "0=0 1=10 2=20 3=30 4=40", toStr );
}

