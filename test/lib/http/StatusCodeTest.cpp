/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StatusCode.h"
using std::string;

TEST_CASE( "StatusCodeTest/testDefault", "[unit]" )
{
	StatusCode code(200);
	assertEquals( 200, code.integer() );
	assertEquals( 200, code );
	assertEquals( "200", code.str() );
	assertEquals( "Success", code.verbose() );

	code = StatusCode::BadRequest;
	assertEquals( 400, code.integer() );
	assertEquals( "Bad Request", code.verbose() );

	StatusCode none;
	assertEquals( 0, none.integer() );
	assertEquals( "Internal Server Error", none.verbose() );
}
