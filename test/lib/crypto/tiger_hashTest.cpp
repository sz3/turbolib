/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "tiger_hash.h"
#include "serialize/base64.h"
using std::string;

TEST_CASE( "tiger_hashTest/testDefault", "[unit]" )
{
	tiger_hash hasher;
	string res = hasher("hello world");
	assertEquals( "H6yxqi0qwZUmfoLblWUFiqHgDYBd5dn1", base64::encode(res) );
}
