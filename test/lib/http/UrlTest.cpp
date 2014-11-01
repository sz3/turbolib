/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "Url.h"
#include "serialize/str_join.h"
using std::string;

namespace {
	template <class T1, class T2>
	std::ostream& operator<<(std::ostream& outstream, const std::pair<T1,T2>& pear)
	{
		outstream << pear.first << "=" << pear.second;
		return outstream;
	}
}

TEST_CASE( "UrlTest/testTokenize", "[unit]" )
{
	Url url("/data/[foo]/bar/barf/");
	assertEquals( "/data/[foo]/bar/barf/", url.str() );
	assertEquals( "/data/[foo]/bar/barf/", url.baseUrl() );
	assertEquals( "data [foo] bar barf", turbo::str::join(url.components()) );
	assertEquals( "", url.queryString() );
	assertEquals( "", turbo::str::join(url.params()) );

	url.append("?thing=hi&spo");
	assertEquals( "/data/[foo]/bar/barf/?thing=hi&spo", url.str() );
	assertEquals( "/data/[foo]/bar/barf/", url.baseUrl() );
	assertEquals( "data [foo] bar barf", turbo::str::join(url.components()) );
	assertEquals( "thing=hi&spo", url.queryString() );
	assertEquals( "spo= thing=hi", turbo::str::join(url.params()) );

	url.append("rt=football");
	assertEquals( "/data/[foo]/bar/barf/?thing=hi&sport=football", url.str() );
	assertEquals( "thing=hi&sport=football", url.queryString() );
	assertEquals( "sport=football thing=hi", turbo::str::join(url.params()) );
}
