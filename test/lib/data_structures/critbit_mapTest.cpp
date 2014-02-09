/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "critbit_map.h"
#include "util/Random.h"
#include <utility>
using std::string;

TEST_CASE( "cribit_mapTest/testIntMap", "[unit]" )
{
	critbit_map<unsigned long long, string> map;

	assertEquals( 2, map.insert({10, "banana"}) );
	assertTrue( map.contains(10) );
	assertEquals( "banana", map.lower_bound(10)->second );

	assertEquals(2, map.insert({20, "orange"}) );
	assertTrue( map.contains(20) );
	assertEquals( "orange", map.lower_bound(20)->second );

	assertEquals(2, map.insert({47, "rocketship"}) );
	assertTrue( map.contains(47) );
	assertEquals( "rocketship", map.lower_bound(47)->second );

	assertEquals(1, map.insert({10, "apple"}) );
	assertTrue( map.contains(10) );
	assertEquals(1, map.remove(10) );
	assertFalse( map.contains(10) );

	assertFalse(map.empty());
	map.clear();
	assertTrue(map.empty());
}

TEST_CASE( "critbit_mapTest/testClassKeyInt_Load", "[unit]" )
{
	critbit_map<unsigned long long, string> map;

	for (int i = 0xFF; i > 0; --i)
	{
		assertEquals( 2, map.insert({i, "banana"}) );

		critbit_map_pair<unsigned long long, string>* pear = map.lower_bound(i);
		assertNotNull( pear );
		assertEquals( i, pear->first );
		assertEquals( "banana", pear->second );
	}

	critbit_map_pair<unsigned long long, string>* pear = map.lower_bound(10);
	assertNotNull( pear );
	assertEquals( 10, pear->first );
	assertEquals( "banana", pear->second );
}
