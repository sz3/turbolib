/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "critbit_map.h"
using std::string;
using turbo::critbit_map;

TEST_CASE( "cribit_mapTest/testIntMap", "[unit]" )
{
	critbit_map<unsigned long long, string> map;

	critbit_map<unsigned long long, string>::pair pear = map.insert({10, "banana"});
	assertTrue( pear );
	assertTrue( map.contains(10) );
	assertEquals( "banana", map.lower_bound(10).second() );

	pear = map.insert({20, "orange"});
	assertTrue( pear );
	assertTrue( map.contains(20) );
	assertEquals( "orange", map.lower_bound(20).second() );

	pear = map.insert({47, "rocketship"});
	assertTrue( pear );
	assertTrue( map.contains(47) );
	assertEquals( "rocketship", map.lower_bound(47).second() );

	pear = map.insert({10, "apple"});
	assertTrue( pear );
	assertEquals(10, pear.first());
	assertEquals("banana", pear.second()); // no overwrite!
	assertTrue( map.contains(10) );
	assertEquals(1, map.remove(10) );
	assertFalse( map.contains(10) );

	assertFalse(map.empty());
	map.clear();
	assertTrue(map.empty());
	assertFalse( map.find(10) );
}

TEST_CASE( "cribit_mapTest/testModifyValue", "[unit]" )
{
	critbit_map<unsigned long long, string> map;

	critbit_map<unsigned long long, string>::pair pear = map.insert({10, "banana"});
	assertTrue( pear );
	assertTrue( map.contains(10) );
	assertEquals( "banana", map.find(10).second() );

	pear.second() = "potato";
	assertEquals( "potato", map.find(10).second() );
}

TEST_CASE( "critbit_mapTest/testClassKeyInt_Load", "[unit]" )
{
	critbit_map<unsigned long long, string> map;

	for (int i = 0xFF; i > 0; --i)
	{
		assertTrue( map.insert({i, "banana"}) );
		critbit_map<unsigned long long, string>::pair pear = map.lower_bound(i);
		assertTrue( pear );
		assertEquals( i, pear.first() );
		assertEquals( "banana", pear.second() );
	}

	critbit_map<unsigned long long, string>::pair pear = map.lower_bound(10);
	assertTrue( pear );
	assertEquals( 10, pear.first() );
	assertEquals( "banana", pear.second() );
}
