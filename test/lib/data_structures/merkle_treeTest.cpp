#include "unittest.h"

#include "merkle_tree.h"
#include "util/Random.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;

TEST_CASE( "merkle_treeTest/testClassKeyInt", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	assertEquals( 2, tree.insert(10, 1337) );
	assertTrue( tree.contains(10) );
	assertEquals( 1337, tree.lower_bound(10)->second );

	assertEquals(2, tree.insert(20, 2048) );
	assertTrue( tree.contains(20) );
	assertEquals( 2048, tree.lower_bound(20)->second );

	assertEquals(2, tree.insert(47, 42) );
	assertTrue( tree.contains(47) );
	assertEquals( 42, tree.lower_bound(47)->second );

	assertEquals(1, tree.insert(10, 15) );
	assertTrue( tree.contains(10) );
	assertEquals(1, tree.remove(10) );
	assertFalse( tree.contains(10) );

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

TEST_CASE( "merkle_treeTest/testClassKeyInt_Load", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	for (int i = 0xFF; i > 0; --i)
	{
		assertEquals( 2, tree.insert(i, 0xf00) );

		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(i);
		assertNotNull( internalNode );
		assertEquals( i, internalNode->first );
		assertEquals( 0xf00, internalNode->second );
	}

	merkle_pair<unsigned, unsigned long long>* node = tree.lower_bound(10);
	assertNotNull( node );
	assertEquals( 10, node->first );
	assertEquals( 0xf00, node->second );
}
