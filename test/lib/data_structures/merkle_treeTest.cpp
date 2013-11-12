#include "unittest.h"

#include "merkle_tree.h"
#include "util/Random.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;

TEST_CASE( "merkle_treeTest/testAddRemoveTreeWalk", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	{
		assertEquals( 2, tree.insert(1337, 1337) );
		assertTrue( tree.contains(1337) );

		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(1337);
		assertNotNull( internalNode );
		assertEquals( 1337, internalNode->second );

		merkle_node<unsigned long long>* parent = internalNode->parent;
		assertNull( parent );
	}

	assertEquals(2, tree.insert(2048, 2048) );
	assertTrue( tree.contains(2048) );
	assertEquals( 2048, tree.lower_bound(2048)->second );

	assertEquals(2, tree.insert(42, 42) );
	assertTrue( tree.contains(42) );
	assertEquals( 42, tree.lower_bound(42)->second );

	{
		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(1337);
		assertNotNull( internalNode );
		assertEquals( 1337, internalNode->second );

		merkle_node<unsigned long long>* parent = internalNode->parent;
		assertNotNull( parent );
		assertEquals( (1337 xor 42), parent->hash );

		parent = parent->parent;
		assertNotNull( parent );
		assertEquals( (1337 xor 42 xor 2048), parent->hash );

		parent = parent->parent;
		assertNull(parent);
	}

	assertEquals(1, tree.insert(1337, 15) );
	assertTrue( tree.contains(1337) );
	assertEquals(1, tree.remove(1337) );
	assertFalse( tree.contains(1337) );

	{
		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(2048);
		assertNotNull( internalNode );
		assertEquals( 2048, internalNode->second );

		merkle_node<unsigned long long>* parent = internalNode->parent;
		assertNotNull( parent );
		assertEquals( (2048 xor 42), parent->hash );

		parent = parent->parent;
		assertNull(parent);
	}

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

TEST_CASE( "merkle_treeTest/testHeavyLoad", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	for (int i = 0xFF; i > 0; --i)
	{
		//std::cout << "insert of element " << i << std::endl;
		assertEquals( 2, tree.insert(i, i*i) );

		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(i);
		assertNotNull( internalNode );
		assertEquals( i, internalNode->first );
		assertEquals( (i*i), internalNode->second );
	}

	merkle_pair<unsigned, unsigned long long>* node = tree.lower_bound(10);
	assertNotNull( node );
	assertEquals( 10, node->first );
	assertEquals( 100, node->second );
}

TEST_CASE( "merkle_treeTest/testHashLookup", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	tree.insert(1337, 1337);
	tree.insert(2048, 2048);
	tree.insert(42, 42);

	// 2048 == ... 0000 1000 0000 0000
	// 1337 == ... 0000 0101 0011 1001
	//   42 == ... 0000 0000 0010 1010

	// ... if only it were that simple. Endianness rains on our parade.
	// 2048 == 0000 0000 | 0000 1000 | ...
	// 1337 == 0011 1001 | 0000 0101 | ...
	//   42 == 0010 1010 | 0000 0000 | ...

	/* so:
	 *           (bit 3)
	 *         /         \
	 *       2048      (bit 4)
	 *                /       \
	 *               42      1337
	 **/

	// root
	unsigned long long hash;
	assertTrue( tree.hash_lookup(merkle_location<unsigned>(0, 0), hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );
	assertTrue( tree.hash_lookup(merkle_location<unsigned>(1337, 0), hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );
	assertTrue( tree.hash_lookup(merkle_location<unsigned>(1337, 2), hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );

	// right side of the first branch (bit 3) -- node
	assertTrue( tree.hash_lookup(merkle_location<unsigned>(1337, 3), hash) );
	assertEquals( (1337 xor 42), hash );

	// left side of the first branch (bit 3) -- leaf
	assertFalse( tree.hash_lookup(merkle_location<unsigned>(2048, 3), hash) );
	assertEquals( 2048, hash );

	// right side of second branch (bit 4) -- leaf
	assertFalse( tree.hash_lookup(merkle_location<unsigned>(1337, 4), hash) );
	assertEquals( 1337, hash );

	// full lookup (32)
	assertFalse( tree.hash_lookup(merkle_location<unsigned>(1337, 32), hash) );
	assertEquals( 1337, hash );

	assertFalse( tree.hash_lookup(merkle_location<unsigned>(1337), hash) );
	assertEquals( 1337, hash );

	// remove 1337
	tree.remove(1337);

	// root again.
	assertTrue( tree.hash_lookup(merkle_location<unsigned>(0, 0), hash) );
	assertEquals( (42 xor 2048), hash );
}

