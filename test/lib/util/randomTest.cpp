/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "random.h"

TEST_CASE( "randomTest/testSelect", "[unit]" )
{
	std::vector<int> numbers;
	for (int i = 0; i < 50; ++i)
		numbers.push_back(i);

	for (int i = 0; i < 50; ++i)
		assertInRange(0, *turbo::random::select(numbers), 49);
}

