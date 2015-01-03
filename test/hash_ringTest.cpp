/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "hash_ring.h"
#include <string>
using turbo::hash_ring;
using namespace std;

TEST_CASE( "hash_ringTest/testStoreAndRetrieve", "[unit]" )
{
	hash_ring<string,string> ring;
	assertEquals( 0, ring.size() );

	// one insert
	ring["foo"] = "bar";
	assertEquals( "bar", ring["foo"] );

	hash_ring<string,string>::circular_iterator it = ring.lower_bound(12345);
	assertTrue( it != ring.end() );
	assertEquals( it->second, "bar");
	assertEquals( it->first, ring.hash("foo") );

	// another
	ring["again"] = "again";
	assertEquals( "again", ring["again"] );

	it = ring.find( ring.hash("again") );
	assertEquals( it->second, "again" );
	assertEquals( it->first, ring.hash("again") );

	// a third
	ring["three"] = "fiddy";
	assertEquals( "fiddy", ring["three"] );

	// change an existing one
	ring["foo"] = "oof!";
	assertEquals( "oof!", ring["foo"] );

	assertEquals( 3, ring.size() );
}

namespace {
	class IdentityHasher
	{
	public:
		using result_type = string;

		result_type operator()(const string& input)
		{
			return input;
		}

	};
}

TEST_CASE( "hash_ringTest/testCustomHash", "[unit]" )
{
	// it's literally... not a hash. But we're just testing the ability to implement a *custom* hash, not a *good* hash.
	using HashRing = hash_ring<string, string, IdentityHasher>;
	HashRing ring;

	ring["foo"] = "bar";
	assertEquals( "bar", ring["foo"] );

	HashRing::circular_iterator it = ring.lower_bound("woo");
	assertTrue( it != ring.end() );
	assertEquals( it->second, "bar");
	assertEquals( it->first, "foo" );
}


TEST_CASE( "hash_ringTest/testWrap", "[unit]" )
{
	// use the identity hash, so the sort order is reliable
	using HashRing = hash_ring<string, int, IdentityHasher>;
	HashRing ring;

	ring["1"] = 1;
	ring["2"] = 2;
	ring["3"] = 3;

	// NOTE: all calls except operator[] and remove() currently expect a hashed value as input.
	//  obviously, in this test we're cheating, so it doesn't matter.
	int i = 1;
	for (HashRing::circular_iterator it = ring.lower_bound("woo"); it != ring.end(); ++it)
	{
		assertTrue( it != ring.end() );
		assertEquals( it->second, i);
		++i;
	}

	i = 1;
	for (HashRing::circular_iterator it = ring.lower_bound("1"); it != ring.end(); ++it)
	{
		assertTrue( it != ring.end() );
		assertEquals( it->second, i);
		++i;
	}

	{
		HashRing::circular_iterator it = ring.lower_bound("3");
		assertTrue( it != ring.end() );
		assertEquals( it->second, 3 );

		++it;
		assertTrue( it != ring.end() );
		assertEquals( it->second, 1 );

		++it;
		assertTrue( it != ring.end() );

		++it;
		assertTrue( it == ring.end() );
	}
}

