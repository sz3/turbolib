#include "unittest.h"

#include "Random.h"
#include <iostream>

TEST_CASE( "RandomTest/testDefault", "description" )
{
	std::vector<int> numbers;
	for (int i = 0; i < 50; ++i)
		numbers.push_back(i);

	for (int i = 0; i < 10; ++i)
		std::cout << *Random::select(numbers.begin(), numbers.end(), numbers.size()) << ", ";
}

