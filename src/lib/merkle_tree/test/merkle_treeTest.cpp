/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "merkle_tree.h"
#include <deque>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;
using turbo::merkle_diff;
using turbo::merkle_point;
using turbo::merkle_tree;

namespace {
	template <typename Container>
	string join(const Container& cont)
	{
		std::stringstream ss;
		auto it = cont.begin();
		if (it != cont.end())
			ss << *it++;
		for (; it != cont.end(); ++it)
			ss << " " << *it;
		return ss.str();
	}

	template <typename Type>
	string str(const Type& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}

TEST_CASE( "merkle_treeTest/testAddRemoveTreeWalk", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	assertTrue( tree.insert(1337, 1337) );
	assertTrue( tree.contains(1337) );
	{
		merkle_tree<unsigned, unsigned long long>::pair pear = tree.lower_bound(1337);
		assertTrue( pear );
		assertEquals( 1337, std::get<0>(pear.second()) );
	}

	assertTrue( tree.insert(2048, 2048) );
	assertTrue( tree.contains(2048) );
	assertEquals( 2048, std::get<0>(tree.lower_bound(2048).second()) );

	assertTrue( tree.insert(42, 42) );
	assertTrue( tree.contains(42) );
	assertEquals( 42, std::get<0>(tree.lower_bound(42).second()) );

	assertTrue( tree.insert(1337, 15) );
	assertTrue( tree.contains(1337) );
	assertEquals( 1337, std::get<0>(tree.find(1337).second()) );
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
		assertTrue( tree.insert(i, i*i) );

		merkle_tree<unsigned, unsigned long long>::pair pear = tree.lower_bound(i);
		assertTrue( pear );
		assertEquals( i, pear.first() );
		assertEquals( (i*i), std::get<0>(pear.second()) );
	}

	merkle_tree<unsigned, unsigned long long>::pair node = tree.lower_bound(10);
	assertTrue( node );
	assertEquals( 10, node.first() );
	assertEquals( 100, std::get<0>(node.second()) );
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
	assertTrue( tree.hash_lookup(0, 0, hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );
	assertTrue( tree.hash_lookup(1337, 0, hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );
	assertTrue( tree.hash_lookup(1337, 2, hash) );
	assertEquals( (1337 xor 42 xor 2048), hash );

	// right side of the first branch (bit 3) -- node
	assertTrue( tree.hash_lookup(1337, 3, hash) );
	assertEquals( (1337 xor 42), hash );

	// left side of the first branch (bit 3) -- leaf
	assertFalse( tree.hash_lookup(2048, 3, hash) );
	assertEquals( 2048, hash );

	// right side of second branch (bit 4) -- leaf
	assertFalse( tree.hash_lookup(1337, 4, hash) );
	assertEquals( 1337, hash );

	// full lookup (32)
	assertFalse( tree.hash_lookup(1337, 32, hash) );
	assertEquals( 1337, hash );

	assertFalse( tree.hash_lookup(1337, 0xFF, hash) );
	assertEquals( 1337, hash );

	// remove 1337
	tree.remove(1337);

	// root again.
	assertTrue( tree.hash_lookup(0, 0, hash) );
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

TEST_CASE( "merkle_treeTest/testTop", "[unit]" )
{
	using mpoint = merkle_point<unsigned, unsigned long long>;
	merkle_tree<unsigned, unsigned long long> tree;

	mpoint nulltop = tree.top();
	assertEquals( 0, nulltop.key );
	assertEquals( 65535, nulltop.keybits );
	assertEquals( 0, nulltop.hash );
	assertTrue( nulltop == mpoint::null() );

	tree.insert(45, 45);
	tree.insert(2048, 2048);

	mpoint top = tree.top();
	assertEquals( 0, top.key );
	assertEquals( 2, top.keybits );
	assertEquals( (2048 xor 45), top.hash );
	assertFalse( top == mpoint::null() );
}

TEST_CASE( "merkle_treeTest/testDiffs", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long> tree;

	// empty tree
	merkle_diff<unsigned, unsigned long long> results = tree.diff({ 0, 0, 0 });
	assertEquals( 1, results.size() );
	assertEquals( results[0], (merkle_point<unsigned, unsigned long long>::null()) );

	tree.insert(32810, 32810);
	tree.insert(2048, 2048);
	tree.insert(42, 42);
	tree.insert(128, 128); // branch at bit 1, because we're about that life

	// like the above,
	//   128 == 1000 0000 | 0000 0000 | ...
	//  2048 == 0000 0000 | 0000 1000 | ...
	//    42 == 0010 1010 | 0000 0000 | ...
	// 32810 == 0010 1010 | 1000 0000 | ...

	/* so:
	 *                 (bit 1)
	 *               /         \
	 *           (bit 3)       128
	 *         /         \
	 *       2048      (bit 9)
	 *                /       \
	 *               42      32810
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
	results = tree.diff({ (32810 xor 42 xor 2048 xor 128), 0, 0 });
	assertEquals( 0, results.size() );
	assertTrue( results.no_difference() );

	{
		// bad hash at root
		results = tree.diff({ 0xF00, 0, 0 });
		assertEquals( 3, results.size() );
		assertTrue( results.traverse() );

		// left child -- location of next lookup
		assertEquals( (32810 xor 42 xor 2048), results[0].hash );
		assertEquals( 2048, results[0].key );
		assertEquals( 2, results[0].keybits );

		// right child
		assertEquals( 128, results[1].hash );
		assertEquals( 128, results[1].key );
		assertEquals( 32, results[1].keybits );

		// passed in location w/ disagreement hash (root, in this case)
		assertEquals( (32810 xor 42 xor 2048 xor 128), results[2].hash );
		assertEquals( 0, results[2].key );
		assertEquals( 0, results[2].keybits );
	}

	{
		// bad hash at pre-branch bit -> returns merkle_location of next branch
		// need_partial_range() == true
		results = tree.diff({ 0xF00, 0, 1 });
		assertEquals( 1, results.size() );
		assertTrue( results.need_partial_range() );

		assertEquals( 2048, results[0].key ); // 2048 with 2nd bit flipped (xor 64)
		assertEquals( 2, results[0].keybits );
	}

	// left side
	results = tree.diff({ (32810 xor 42 xor 2048), 2048, 2 });
	assertEquals( 0, results.size() );
	assertTrue( results.no_difference() );

	{
		// bad hash at branch bit -> we have diffs
		results = tree.diff({ 0xF00, 2048, 2 });
		assertEquals( 3, results.size() );
		assertTrue( results.traverse() );

		// left child
		assertEquals( 2048, results[0].hash );
		assertEquals( 2048, results[0].key );
		assertEquals( 32, results[0].keybits );

		// right child -- location of next lookup
		assertEquals( (42 xor 32810), results[1].hash );
		assertEquals( 42, results[1].key );
		assertEquals( 8, results[1].keybits );

		// passed in location w/ disagreement hash
		assertEquals( (32810 xor 42 xor 2048), results[2].hash );
		assertEquals( 2048, results[2].key );
		assertEquals( 2, results[2].keybits );
	}

	results = tree.diff({ 2048, 2048, 3 });
	assertEquals( 0, results.size() );
	assertTrue( results.no_difference() );

	{
		// bad hash on leaf -> one diff.
		results = tree.diff({ 0xF00, 2048, 3 });
		assertEquals( 1, results.size() );
		assertTrue( results.need_partial_range() );
		assertEquals( 2048, results[0].hash );
		assertEquals( 2048, results[0].key );
		assertEquals( 32, results[0].keybits );
	}

	// right child, using key from above
	results = tree.diff({ (42 xor 32810), 42, 8 });
	assertEquals( 0, results.size() );

	{
		// bad hash, exact bit match
		results = tree.diff({ 0xF00, 42, 8 });
		assertEquals( 3, results.size() );

		// left
		assertEquals( 42, results[0].hash );
		assertEquals( 42, results[0].key );
		assertEquals( 32, results[0].keybits );

		// right
		assertEquals( 32810, results[1].hash );
		assertEquals( 32810, results[1].key );
		assertEquals( 32, results[1].keybits );

		// passed in location w/ disagreement hash
		assertEquals( (42 xor 32810), results[2].hash );
		assertEquals( 42, results[2].key );
		assertEquals( 8, results[2].keybits );
	}

	{
		// bad hash at pre-branch bit
		results = tree.diff({ 0xF00, 42, 7 });
		assertEquals( 1, results.size() );
		assertTrue( results.need_partial_range() );

		assertEquals( 42, results[0].key );
		assertEquals( 8, results[0].keybits );
	}

	// look up a leaf
	results = tree.diff({ 42, 42, 32 });
	assertEquals( 0, results.size() );

	{
		// leaf bad hash
		results = tree.diff({ 0xF00, 42, 32 });
		assertEquals( 1, results.size() );
		assertTrue( results.need_exchange() );

		assertEquals( 42, results[0].hash );
		assertEquals( 42, results[0].key );
		assertEquals( 32, results[0].keybits );
	}
}

TEST_CASE( "merkle_treeTest/testDiffTraverse", "[unit]" )
{
	using mpoint = merkle_point<unsigned, unsigned long long>;
	using mdiff = merkle_diff<unsigned, unsigned long long>;
	merkle_tree<unsigned, unsigned long long> tree;

	tree.insert(45, 45);
	tree.insert(2048, 2048);
	tree.insert(42, 42);
	tree.insert(64, 64);

	// like the above,
	// 2048 == 0000 0000 | 0000 1000 | ...
	//   45 == 0010 1101 | 0000 0000 | ...
	//   42 == 0010 1010 | 0000 0000 | ...
	//   64 == 0100 0000 | 0000 0000 | ...

	/* so:
	 *                 (bit 2)
	 *               /         \
	 *           (bit 3)        64
	 *         /         \
	 *       2048      (bit 6)
	 *                /       \
	 *               42       45
	 **/

	mpoint top = tree.top();
	assertEquals( 0, top.key );
	assertEquals( 1, top.keybits );
	assertEquals( (2048 xor 45 xor 42 xor 64), top.hash );

	mdiff results = tree.diff(top);
	assertEquals( 0, results.size() );
	results = tree.diff( top.copy(0xF00) );
	assertEquals( 3, results.size() );

	mpoint leftAtBit2 = results[0];
	results = tree.diff( leftAtBit2 );
	assertEquals( 0, results.size() );
	results = tree.diff( leftAtBit2.copy(0xF00) );
	assertEquals( 3, results.size() );

	mpoint rightAtBit3 = results[1];
	results = tree.diff( rightAtBit3 );
	assertEquals( 0, results.size() );
	results = tree.diff( rightAtBit3.copy(0xF00) );
	assertEquals( 3, results.size() );

	mpoint leaf42 = results[0];
	assertEquals( 42, leaf42.hash );
	mpoint leaf45 = results[1];
	assertEquals( 45, leaf45.hash );
}

TEST_CASE( "merkle_treeTest/testWithPayload", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long, string> tree;

	tree.insert(1, 10, "one");
	tree.insert(2, 20, "two");

	merkle_point<unsigned, unsigned long long> top = tree.top();
	assertEquals( (10 xor 20), top.hash );

	merkle_tree<unsigned, unsigned long long, string>::pair elem1 = tree.lower_bound(1);
	assertEquals(    10, std::get<0>(elem1.second()) );
	assertEquals( "one", std::get<1>(elem1.second()) );

	merkle_tree<unsigned, unsigned long long, string>::pair elem2 = tree.lower_bound(2);
	assertEquals(   20,  std::get<0>(elem2.second()) );
	assertEquals( "two", std::get<1>(elem2.second()) );
}

TEST_CASE( "merkle_treeTest/testEnumerate", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long, string> tree;

	tree.insert(1, 10, "one");
	tree.insert(2, 20, "two");
	tree.insert(3, 30, "three");

	std::vector<string> words;
	auto fun = [&](unsigned, unsigned long long, const string& payload){ words.push_back(payload); return true; };
	tree.enumerate(fun, 1, 3);

	assertEquals( "one two three", join(words) );
}

TEST_CASE( "merkle_treeTest/testEnumerate.Empty", "[unit]" )
{
	merkle_tree<unsigned, unsigned long long, string> tree;

	std::vector<string> words;
	auto fun = [&](unsigned, unsigned long long, const string& payload){ words.push_back(payload); return true; };
	tree.enumerate(fun, 1, 3);

	assertEquals( "", join(words) );
}

TEST_CASE( "merkle_treeTest/testEnumerate.Lots", "[unit]" )
{
	//  20 == 0001 0100 | 0000 0000 | ...
	// 276 == 0001 0100 | 0000 0001 | ...
	// 532 == 0001 0100 | 0000 0010 | ...
	// 788 == 0001 0100 | 0000 0011 | ...

	merkle_tree<unsigned, unsigned long long, string> tree;

	for (unsigned i = 0; i < 1000; ++i)
		tree.insert(i, i*10, str(i));

	std::vector<string> words;
	auto fun = [&](unsigned, unsigned long long, const string& payload){ words.push_back(payload); return true; };

	tree.enumerate(fun, 20, 26);
	assertEquals( "20 276 532 788 21 277 533 789 "
				  "22 278 534 790 23 279 535 791 "
				  "24 280 536 792 25 281 537 793 "
				  "26", join(words) );

	words.clear();
	tree.enumerate(fun, 1, 2);
	assertEquals( "1 257 513 769 2", join(words) );

	words.clear();
	tree.enumerate(fun, 52, 52);
	assertEquals( "52", join(words) );

	words.clear();
	tree.enumerate(fun, 100000, 100000);
	assertEquals( "", join(words) );
}

TEST_CASE( "merkle_treeTest/testEnumerate.Stop", "[unit]" )
{
	//  20 == 0001 0100 | 0000 0000 | ...
	// 276 == 0001 0100 | 0000 0001 | ...
	// 532 == 0001 0100 | 0000 0010 | ...
	// 788 == 0001 0100 | 0000 0011 | ...

	merkle_tree<unsigned, unsigned long long, string> tree;

	for (unsigned i = 0; i < 1000; ++i)
		tree.insert(i, i*10, str(i));

	std::vector<string> words;
	auto fun = [&](unsigned, unsigned long long, const string& payload){ words.push_back(payload); return words.size() < 5; };

	tree.enumerate(fun, 20, 25);
	assertEquals( "20 276 532 788 21", join(words) );
}
