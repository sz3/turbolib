#include "catch.hpp"

#include "FixedPointNumber.h"

TEST_CASE( "FixedPointNumberTest/testDefault", "description" )
{
	FixedPointNumber<> num(4);
	REQUIRE( num.rawvalue() == 4096 );
	REQUIRE( num.asInt() == 4 );
	REQUIRE( num.remainder() == 0 );

	num.fromFloat(12.25);
	REQUIRE( num.asInt() == 12 );
	REQUIRE( num.remainder() == 256 );
	REQUIRE( num.asDouble() == 12.25 );

	fixfloat flow;
	flow.fromFloat(12.25);
	REQUIRE( flow.rawvalue() == num.rawvalue() );


	FixedPointNumber<long long, 20> longnum;
	longnum.fromFloat(100.5);
	REQUIRE( longnum.asInt() == 100 );
	REQUIRE( longnum.remainder() == 524288 );
	REQUIRE( longnum.asDouble() == 100.5 );

	fixdouble dub;
	dub.fromFloat(100.5);
	REQUIRE( dub.rawvalue() == longnum.rawvalue() );

	std::cerr << dub << std::endl;
}

TEST_CASE( "FixedPointNumberTest/testNegative", "description" )
{
	FixedPointNumber<> neg(-2);
	REQUIRE( neg.rawvalue() == -2048 );
	REQUIRE( neg.asInt() == -2 );
	REQUIRE( neg.remainder() == 0 );
	REQUIRE( neg.asDouble() == -2 );

	neg.fromFloat(-0.5);
	REQUIRE( neg.rawvalue() == -512 );
	REQUIRE( neg.asInt() == -1 ); // truncates down to the lower digit.
	REQUIRE( neg.remainder() == 512 );
	REQUIRE( neg.asDouble() == -0.5 );
}

TEST_CASE( "FixedPointNumberTest/testAdditionSubstraction", "description" )
{

}

TEST_CASE( "FixedPointNumberTest/testMultiplication", "description" )
{
	FixedPointNumber<long, 10> num(1);

	num *= 2;
	REQUIRE( num.rawvalue() == 2048 );
	REQUIRE( num.asInt() == 2 );
	REQUIRE( num.remainder() == 0 );

	num = 2047; // 2^11-1
	num *= 1024;
	REQUIRE( num.asInt() == 2096128 );

	num.fromFloat(1.41);
	num *= FixedPointNumber<>().fromFloat(1.42);
	REQUIRE( num.asDouble() == 2 );
}

TEST_CASE( "FixedPointNumberTest/testDivision", "description" )
{
	FixedPointNumber<> num(6);

	num /= FixedPointNumber<>().fromFloat(1.5);
	REQUIRE( num.rawvalue() == 4096 );
	REQUIRE( num.asInt() == 4 );
	REQUIRE( num.remainder() == 0 );

	num = 4;
	num /= 3;
	REQUIRE( num.rawvalue() == 1365 );
	REQUIRE( num.asInt() == 1 );
	REQUIRE( num.remainder() == 341 );

	num = 4;
	num /= 16;
	REQUIRE( num.asDouble() == 0.25 );
	REQUIRE( num.rawvalue() == 256 );

	num = 0x80000; // 2^19
	num /= 8;
	REQUIRE( num.asInt() == 0x10000 );

	num = 0;
	num /= FixedPointNumber<>().fromFloat(0.0001);
	REQUIRE( num.rawvalue() == 0 );
}

