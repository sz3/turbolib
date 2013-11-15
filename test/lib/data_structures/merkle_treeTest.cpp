#include "unittest.h"

#include "merkle_tree.h"
#include "util/Random.h"
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;

TEST_CASE( "merkle_treeTest/testAddRemoveTreeWalk", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	assertEquals( 2, tree.insert(1337, 1337) );
	assertTrue( tree.contains(1337) );
	{
		merkle_pair<unsigned, unsigned long long>* internalNode = tree.lower_bound(1337);
		assertNotNull( internalNode );
		assertEquals( 1337, internalNode->second );
	}

	assertEquals(2, tree.insert(2048, 2048) );
	assertTrue( tree.contains(2048) );
	assertEquals( 2048, tree.lower_bound(2048)->second );

	assertEquals(2, tree.insert(42, 42) );
	assertTrue( tree.contains(42) );
	assertEquals( 42, tree.lower_bound(42)->second );

	assertEquals(1, tree.insert(1337, 15) );
	assertTrue( tree.contains(1337) );
	assertEquals(1, tree.remove(1337) );
	assertFalse( tree.contains(1337) );

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


TEST_CASE( "merkle_treeTest/testCalcKeybits", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	assertEquals( 1, tree.keybits(0, 128) );
	assertEquals( 2, tree.keybits(0, 64) );
	assertEquals( 3, tree.keybits(0, 32) );
	assertEquals( 4, tree.keybits(0, 16) );
	assertEquals( 5, tree.keybits(0, 8) );
	assertEquals( 6, tree.keybits(0, 4) );
	assertEquals( 7, tree.keybits(0, 2) );
	assertEquals( 8, tree.keybits(0, 1) );

	assertEquals( 9, tree.keybits(1, 128) );
	assertEquals( 17, tree.keybits(2, 128) );
	assertEquals( 161, tree.keybits(20, 128) );
	assertEquals( 168, tree.keybits(20, 1) );
}


TEST_CASE( "merkle_treeTest/testDiffs", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	tree.insert(45, 45);
	tree.insert(2048, 2048);
	tree.insert(42, 42);

	// like the above,
	// 2048 == 0000 0000 | 0000 1000 | ...
	//   45 == 0010 1101 | 0000 0000 | ...
	//   42 == 0010 1010 | 0000 0000 | ...

	/* so:
	 *           (bit 3)
	 *         /         \
	 *       2048      (bit 6)
	 *                /       \
	 *               42       45
	 **/

	/*
	 * diff cases:
	 *  1) empty
	 *  2) no diff
	 *  3) leaf diff -> 1 key is disparate. 1 hash. Need 3rd party to tell us what's wrong.
	 *  4) branch diff, keybits !=. 0 hashes, just the missing branch. If my keybits > than query, I'm missing information on anything in the range (key ^ branch critbit)
	 *  5) branch diff, keybits !=. 2 hashes. If my keybits < than query, other party is missing said info
	 *  6) branch diff, keybits ==. 2 hashes. Recurse to right, left sides.
	 */

	// root
	std::deque< merkle_point<unsigned, unsigned long long> > results = tree.diff( merkle_location<unsigned>(0, 0), (45 xor 42 xor 2048) );
	assertEquals( 0, results.size() );


	{
		// bad hash at pre-branch bit -> returns merkle_location of (implied) missing branch
		results = tree.diff( merkle_location<unsigned>(0, 1), 0xF00 );
		assertEquals( 1, results.size() );

		assertEquals( 0x4800, results[0].location.key );
		assertEquals( 1, results[0].location.keybits );
	}

	{
		// bad hash at branch bit -> we have diffs
		results = tree.diff( merkle_location<unsigned>(0, 2), 0xF00 );
		assertEquals( 2, results.size() );

		// left child
		assertEquals( 2048, results[0].hash );
		assertEquals( 2048, results[0].location.key );
		assertEquals( 32, results[0].location.keybits );

		// right child
		assertEquals( (42 xor 45), results[1].hash );
		assertEquals( 2080, results[1].location.key ); // 2048 xor 32
		assertEquals( 6, results[1].location.keybits );
	}

	// left side
	results = tree.diff( merkle_location<unsigned>(2048, 2), (45 xor 42 xor 2048) );
	assertEquals( 0, results.size() );
	results = tree.diff( merkle_location<unsigned>(2048, 3), 2048 );
	assertEquals( 0, results.size() );

	{
		// bad hash on leaf -> one diff.
		results = tree.diff( merkle_location<unsigned>(2048, 3), 0xF00 );
		assertEquals( 1, results.size() );
		assertEquals( 2048, results[0].hash );
		assertEquals( 2048, results[0].location.key );
		assertEquals( 32, results[0].location.keybits );
	}

	// right child, using derived key from above
	results = tree.diff( merkle_location<unsigned>(2080, 5), (42 xor 45) );
	assertEquals( 0, results.size() );

	{
		// bad hash, exact bit match
		results = tree.diff( merkle_location<unsigned>(2080, 5), 0xF00 );
		assertEquals( 2, results.size() );

		// left
		assertEquals( 42, results[0].hash );
		assertEquals( 42, results[0].location.key );
		assertEquals( 32, results[0].location.keybits );

		// right
		assertEquals( 45, results[1].hash );
		assertEquals( 46, results[1].location.key ); // 42 xor 4
		assertEquals( 32, results[1].location.keybits );
	}
}

