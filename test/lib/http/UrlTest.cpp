/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "Url.h"
#include "serialize/StringUtil.h"
using std::string;

TEST_CASE( "UrlTest/testTokenize", "[unit]" )
{
	Url url("/data/[foo]/bar/barf/");
	assertEquals( "/data/[foo]/bar/barf/", url.str() );
	assertEquals( "/data/[foo]/bar/barf/", url.baseUrl() );
	assertEquals( "", url.queryString() );
	assertEquals( "data [foo] bar barf", StringUtil::join(url.components()) );

	url.append("?thing=hi&spo");
	url.append("rt=football");
}
