#include "catch.hpp"

#define SOOPER_DEBUG
#include "heapsort.h"
#include "mergesort.h"
#include "event/Timer.h"
#include <chrono>
#include <random>
#include <sstream>
#include <vector>
using std::vector;

namespace {
	int compareMyType(const void* a, const void* b)
	{
		if ( *(unsigned*)a <  *(unsigned*)b ) return -1;
		if ( *(unsigned*)a == *(unsigned*)b ) return 0;
		if ( *(unsigned*)a >  *(unsigned*)b ) return 1;
	}

	vector<int> generateRandomBuffer(int size)
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);
		std::uniform_int_distribution<int> distribution(0,size);

		vector<int> result;
		result.resize(size);
		for (int i = 0; i < size; ++i)
			result[i] = distribution(generator);
		return result;
	}

	std::ostream& operator<<(std::ostream& outstream, const vector<int>& vec)
	{
		for (vector<int>::const_iterator it = vec.begin(); it != vec.end(); ++it)
			outstream << *it << ",";
		return outstream;
	}

	std::string stl_join(const vector<int>& vec)
	{
		std::stringstream str;
		str << vec;
		return str.str();
	}
}

TEST_CASE( "SortCompareTest/testDefault", "default" )
{
	int BUFSIZE = 5000;
	int rounds = 6;
	for (int r = 1; r <= rounds; ++r)
	{
		std::cout << std::endl;
		std::cout << " **** run " << r << " ****" << std::endl;

		// make buffer 4 times bigger each time through
		int iterbuffsize = BUFSIZE;
		for (int i = 1; i < r; ++i)
			iterbuffsize *= 4;

		vector<int> base = generateRandomBuffer(iterbuffsize);
		/*for (int i = 0; i < base.size(); ++i)
		{
			if (i%20 == 0)
				std::cout << std::endl;
			else
				std::cout << ", ";
			std::cout << base[i];
		}
		std::cout << std::endl;*/

		vector<int> qsortBuff(base);
		Timer tQ;
		qsort(&qsortBuff[0], base.size(), sizeof(int), &compareMyType);
		std::cout << "qsort     elapsed: " << tQ.micros() << std::endl;

		{
			vector<int> heapBuff(base);
			Timer tH;
			heapsort(&heapBuff[0], base.size());
			std::cout << "heapsort  elapsed: " << tH.micros() << std::endl;

			REQUIRE( stl_join(heapBuff) == stl_join(qsortBuff) );
		}

		{
			vector<int> mergeBuff(base);
			Timer tM;
			bool mergeResult = mergesort(&mergeBuff[0], base.size());
			std::cout << "mergesort elapsed: " << tM.micros() << std::endl;

			REQUIRE( mergeResult );
			REQUIRE( stl_join(mergeBuff) == stl_join(qsortBuff) );
		}
	}
}

