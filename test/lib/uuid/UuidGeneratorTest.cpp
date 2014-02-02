/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "UuidGenerator.h"
using std::string;

// just a smoke test.
TEST_CASE( "UuidGeneratorTest/testGenerate", "[unit]" )
{
	UuidGenerator uuidGen;
	string one = uuidGen.generate();
	string two = uuidGen.generate();

	assertTrue( one != two );
	assertEquals( 36, one.size() );
	assertEquals( 36, two.size() );
}
