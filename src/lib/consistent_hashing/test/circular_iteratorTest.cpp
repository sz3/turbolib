/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "circular_iterator.h"
#include <set>
#include <string>
using turbo::circular_iterator;
using namespace std;

TEST_CASE( "circular_iteratorTest/testLoop", "[unit]" )
{
	set<int> foo = {1, 2, 3};
	string result;

	{
		circular_iterator<set<int>::const_iterator> it( foo.begin(), foo.begin(), foo.end() );
		assertTrue( it != foo.end() );
		assertEquals( 1, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 2, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 3, *it );

		++it;
		assertTrue( it == foo.end() );
	}

	{
		circular_iterator<set<int>::const_iterator> it( foo.find(2), foo.begin(), foo.end() );
		assertTrue( it != foo.end() );
		assertEquals( 2, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 3, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 1, *it );

		++it;
		assertTrue( it == foo.end() );
	}

	{
		circular_iterator<set<int>::const_iterator> it( foo.find(3), foo.begin(), foo.end() );
		assertTrue( it != foo.end() );
		assertEquals( 3, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 1, *it );

		++it;
		assertTrue( it != foo.end() );
		assertEquals( 2, *it );

		++it;
		assertTrue( it == foo.end() );
	}
}


TEST_CASE( "circular_iteratorTest/testLoop2", "[unit]" )
{
	set<string> foo = {"aaa", "bbb", "ccc", "ddd", "eee"};
	string result;

	circular_iterator<set<string>::const_iterator> it( foo.lower_bound("ddd"), foo.begin(), foo.end() );
	for (; it != foo.end(); ++it)
		result += *it;
	assertEquals( "dddeeeaaabbbccc", result );
}
