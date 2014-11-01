/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "FunctionChainer.h"
#include "serialize/str_join.h"
#include <deque>
#include <functional>
#include <string>
using std::deque;
using std::string;
using namespace std::placeholders;

namespace {
	class FunMeister
	{
	public:
		void push(unsigned num, string val)
		{
			_numbers.push_back(num);
			_strings.push_back(val);
		}

	public:
		deque<unsigned> _numbers;
		deque<string> _strings;
	};
}

TEST_CASE( "FunctionChainerTest/testDefault", "[unit]" )
{
	deque<string> strresults;
	auto fun = [&] (unsigned num, string val) { strresults.push_back(val); };

	FunctionChainer<unsigned, string> chain(fun);

	deque<unsigned> numresults;
	auto another = [&] (unsigned num, string val) { numresults.push_back(num); };
	chain.add( another );

	FunMeister meister;
	chain.add( std::bind( &FunMeister::push, &meister, _1, _2 ) );

	auto chainFun = chain.generate();

	chainFun(1, "one");
	chainFun(2, "two");
	chainFun(3, "three");

	assertEquals( "one two three", turbo::str::join(strresults) );
	assertEquals( "1 2 3", turbo::str::join(numresults) );
	assertEquals( "1 2 3", turbo::str::join(meister._numbers) );
	assertEquals( "one two three", turbo::str::join(meister._strings) );
}

