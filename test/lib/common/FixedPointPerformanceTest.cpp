#include "catch.hpp"

#include "FixedPointNumber.h"
#include "event/Timer.h"
#include <functional>
#include <string>
using std::string;

namespace {
	template <typename T>
	void add(unsigned n)
	{
		T val = 0;
		for (unsigned i = 0; i < n; ++i)
			val += 1;
		REQUIRE( val == n );
	}

	template <typename T>
	void multiply(unsigned n)
	{
		T val;
		for (unsigned i = 0; i < n; ++i)
		{
			val = 2;
			val *= i;
		}
		REQUIRE( val == 2*(n-1) );
	}

	template <typename T>
	void divide(unsigned n)
	{
		T val;
		for (unsigned i = 1; i <= n; ++i)
		{
			val = n;
			val /= i;
		}
		REQUIRE( val == 1 );
	}

	void looper(const std::function<void()>& fun, const string& what)
	{
		Timer t;
		const unsigned iterations = 100;
		for (int i = 0; i < iterations; ++i)
			fun();
		std::cerr << what << ": " << t.micros() << " us (" << t.millis() << " ms)" << std::endl;
	}
}

TEST_CASE( "FixedPointPerformanceTest/testBenchmark", "description" )
{
	const unsigned count = 12345678;

	looper(std::bind(&add<unsigned>, count), "unsigned addition");
	looper(std::bind(&add<unsigned long long>, count), "ull addition");
	/*looper(std::bind(&add<float>, count), "float addition");
	looper(std::bind(&add<fixfloat>, count), "fixfloat addition");
	looper(std::bind(&add<double>, count), "double addition");
	looper(std::bind(&add<fixdouble>, count), "fixdouble addition");

	looper(std::bind(&multiply<unsigned>, count), "unsigned multiply");
	looper(std::bind(&multiply<unsigned long long>, count), "ull multiply");
	looper(std::bind(&multiply<float>, count), "float multiply");
	looper(std::bind(&multiply<fixfloat>, count), "fixfloat multiply");
	looper(std::bind(&multiply<double>, count), "double multiply");
	looper(std::bind(&multiply<fixdouble>, count), "fixdouble multiply");

	looper(std::bind(&divide<unsigned>, count), "unsigned divide");
	looper(std::bind(&divide<unsigned long long>, count), "ull divide");
	looper(std::bind(&divide<float>, count), "float divide");
	looper(std::bind(&divide<fixfloat>, count), "fixfloat divide");
	looper(std::bind(&divide<double>, count), "double divide");
	looper(std::bind(&divide<fixdouble>, count), "fixdouble divide");
	*/
}

