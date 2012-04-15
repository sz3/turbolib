#include "catch.hpp"

#include "FixedPointNumber.h"

TEST_CASE( "FixedPointNumberTest/testDefault", "description" )
{
	FixedPointNumber<> num(4);
	REQUIRE( num.rawvalue() == 4096 );
	REQUIRE( num.whole() == 4 );
	REQUIRE( num.remainder() == 0 );

	num = 12.25;
	REQUIRE( num.whole() == 12 );
	REQUIRE( num.remainder() == 256 );

	fixfloat flow;
	flow = 12.25;
	REQUIRE( flow.rawvalue() == num.rawvalue() );


	FixedPointNumber<long long, 20> longnum;
	longnum = 100.5;
	REQUIRE( longnum.whole() == 100 );
	REQUIRE( longnum.remainder() == 524288 );

	fixdouble dub;
	dub = 100.5;
	REQUIRE( dub.rawvalue() == longnum.rawvalue() );
}

TEST_CASE( "FixedPointNumberTest/testNegative", "description" )
{
	FixedPointNumber<> neg(-2);
	REQUIRE( neg.rawvalue() == -2048 );
	REQUIRE( neg.whole() == -2 );
	REQUIRE( neg.remainder() == 0 );

	neg = -0.5;
	REQUIRE( neg.rawvalue() == -512 );
	REQUIRE( neg.whole() == -1 ); // truncates down to the lower digit.
	REQUIRE( neg.remainder() == 512 );
}
