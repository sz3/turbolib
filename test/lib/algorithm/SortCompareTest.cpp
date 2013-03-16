#include "catch.hpp"

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
	//int BUFSIZE = 500000;
	int BUFSIZE = 20;
	int rounds = 4;
	for (int r = 1; r <= rounds; ++r)
	{
		vector<int> base = generateRandomBuffer(BUFSIZE*r);
		/*for (int i = 0; i < base.size(); ++i)
		{
			if (i%20 == 0)
				std::cout << std::endl;
			else
				std::cout << ", ";
			std::cout << base[i];
		}
		std::cout << std::endl;*/

		vector<int> heapBuff(base);
		Timer tH;
		heapsort(&heapBuff[0], base.size());
		std::cout << "heapsort  elapsed: " << tH.micros() << std::endl;

		vector<int> mergeBuff(base);
		Timer tM1;
		vector<int> mergeWorkBuff;
		mergeWorkBuff.resize(base.size());
		Timer tM2;
		mergesort(&mergeBuff[0], &mergeWorkBuff[0], base.size());
		std::cout << "mergesort elapsed: " << tM1.micros() << " , " << tM2.micros() << std::endl;

		vector<int> qsortBuff(base);
		Timer tQ;
		qsort(&qsortBuff[0], base.size(), sizeof(int), &compareMyType);
		std::cout << "qsort     elapsed: " << tQ.micros() << std::endl;

		REQUIRE( stl_join(heapBuff) == stl_join(qsortBuff) );
		REQUIRE( stl_join(mergeBuff) == stl_join(qsortBuff) );
	}
}

