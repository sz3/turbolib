/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "Url.h"
using std::string;

namespace {
	template <class T1, class T2>
	std::ostream& operator<<(std::ostream& outstream, const std::pair<T1,T2>& pear)
	{
		outstream << pear.first << "=" << pear.second;
		return outstream;
	}
}
#include "serialize/str_join.h"

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

TEST_CASE( "UrlTest/testEncode", "[unit]" )
{
	assertEquals( "foobar", Url::encode("foobar") );
	assertEquals( "foo%2fbar", Url::encode("foo/bar") );
	assertEquals( "1%40too%20OP%3f%5b%5d", Url::encode("1@too OP?[]") );
}

TEST_CASE( "UrlTest/testDecode", "[unit]" )
{
	assertEquals( "foobar", Url::decode("foobar") );
	assertEquals( "foo/bar", Url::decode("foo%2Fbar") );
	assertEquals( "foo bar%", Url::decode("foo%20bar%25") );
	assertEquals( "1@too OP?[]", Url::decode("1%40too%20OP%3f%5b%5d") );

	// the mean cases
	assertEquals( "foobar%", Url::decode("foobar%") );
	assertEquals( "foobar%%", Url::decode("foobar%%") );
	assertEquals( "foobar%%%", Url::decode("foobar%%%") );
}
