/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "serialize/base64.h"
using std::string;

TEST_CASE( "base64Test/testToFrom", "[unit]" )
{
	string encoded = base64::encode("foo");
	assertEquals( "Paxj", encoded );
	assertEquals( "foo", base64::decode(encoded) );

	encoded = base64::encode("Hello");
	assertEquals( "I6LgR6w=", encoded );
	assertEquals( "Hello", base64::decode(encoded) );

	string value;
	unsigned char c = 0xFF;
	value.resize(4, c);
	encoded = base64::encode(value);
	assertEquals( value, base64::decode(encoded) );
}
