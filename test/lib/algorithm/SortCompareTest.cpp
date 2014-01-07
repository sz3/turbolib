/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "catch.hpp"

//#define SOOPER_DEBUG
#include "heapsort.h"
#include "mergesort.h"
#include "serialize/StringUtil.h"
#include "time/Timer.h"
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
}

TEST_CASE( "SortCompareTest/testDefault", "default" )
{
	int BUFSIZE = 500;
	int rounds = 6;
	for (int r = 1; r <= rounds; ++r)
	{
		// make buffer 4 times bigger each time through
		int iterbuffsize = BUFSIZE;
		for (int i = 1; i < r; ++i)
			iterbuffsize *= 4;

		std::cout << std::endl;
		std::cout << " **** run " << r << " ****" << std::endl;
		std::cout << "      buffer size is " << iterbuffsize << std::endl;

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
		std::cout << "qsort     elapsed: " << tQ.millis() << std::endl;

		{
			vector<int> heapBuff(base);
			Timer tH;
			heapsort(&heapBuff[0], base.size());
			std::cout << "heapsort  elapsed: " << tH.millis() << std::endl;

			REQUIRE( StringUtil::stlJoin(heapBuff) == StringUtil::stlJoin(qsortBuff) );
		}

		{
			vector<int> mergeBuff(base);
			Timer tM;
			bool mergeResult = mergesort(&mergeBuff[0], base.size());
			std::cout << "mergesort elapsed: " << tM.millis() << std::endl;

			REQUIRE( mergeResult );
			REQUIRE( StringUtil::stlJoin(mergeBuff) == StringUtil::stlJoin(qsortBuff) );
		}
	}
}

