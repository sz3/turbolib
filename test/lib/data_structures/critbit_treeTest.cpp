/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "critbit_tree.h"
#include "serialize/StringUtil.h"
#include "util/Random.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::string;

TEST_CASE( "critbit_treeTest/testInsertLookup", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	std::cout << "insert? " << tree.insert("one") << std::endl;
	std::cout << "find? " << (unsigned)tree.lower_bound("one") << std::endl;
	std::cout << "contains? " << tree.contains("one") << std::endl;

	assertEquals(2, tree.insert("two") );
	assertTrue( tree.contains("two") );

	assertEquals(2, tree.insert("forty-seven") );
	assertTrue( tree.contains("forty-seven") );

	assertEquals(1, tree.insert("one") );
	assertTrue( tree.contains("one") );

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

TEST_CASE( "critbit_treeTest/testManyInserts", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	for (int i = 0; i < 15; ++i)
	{
		std::stringstream ss;
		ss << i;
		assertEquals( 2, tree.insert(ss.str().c_str()) );
		assertTrue( tree.contains(ss.str().c_str()) );
	}
}

TEST_CASE( "critbit_treeTest/testRemove", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	for (int i = 0; i < 15; ++i)
	{
		std::stringstream ss;
		ss << i;
		assertEquals( 2, tree.insert(ss.str().c_str()) );
		assertTrue( tree.contains(ss.str().c_str()) );
	}

	assertEquals( 2, tree.insert("banana") );
	assertTrue( tree.contains("banana") );
	assertEquals( 1, tree.remove("banana") );
	assertFalse( tree.contains("banana") );
	assertEquals( 0, tree.remove("banana") );

	for (int i = 0; i < 15; ++i)
	{
		std::stringstream ss;
		ss << i;
		assertEquals( 1, tree.remove(ss.str().c_str()) );
	}
	assertTrue(tree.empty());
}

namespace {
	void print_cstr_tree(critbit_node_ptr<char, critbit_node> root, string label)
	{
		if (root.isNull())
			return;

		if (root.isLeaf())
			std::cout << label << " leaf: " << root.leaf() << std::endl;
		else
		{
			critbit_node* node = root.node();
			std::cout << label << " node at " << node->byte << "," << (unsigned)(node->otherbits ^ 0xFF) << std::endl;
			print_cstr_tree(node->child[0], label + " left");
			print_cstr_tree(node->child[1], label + " right");
		}
	}
}

TEST_CASE( "critbit_treeTest/testPrefixLookup", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("two") );
	assertEquals(2, tree.insert("forty-two") );
	assertEquals(2, tree.insert("forty-seven") );
	assertEquals(2, tree.insert("one") );

	/*
	 * for review:
	 *   't' == 116 == 01110100
	 *   'f' == 102 == 01100110
	 *   'o' == 111 == 01101111
	 *
	 * So tree should look like this,
	 *  where 0:16 means we branch on the 5th (10000 == 16) bit of the 1st (0th) byte:
	 *
	 *           0:16
	 *         /      \
	 *       0:8      two
	 *      /    \
	 *    6:4    one
	 *   .....
	 */

	using node_ptr = critbit_node_ptr<char, critbit_node>;
	{
		// use bit mask to specify which parts of the last byte we don't care about
		// 000s == exact match
		node_ptr elem = tree.subtree("o");
		assertFalse( elem.isNode() );
		assertStringsEqual( "one", elem.leaf() );
	}
	{
		node_ptr elem = tree.subtree("o", 0xF); // don't match the final 4 bits
		assertTrue( elem.isNode() );
		node_ptr right = elem.node()->child[1];
		assertStringsEqual( "one", right.leaf() );

		node_ptr left = elem.node()->child[0];
		assertTrue(left.isNode());
		node_ptr leftleft = left.node()->child[0];
		assertStringsEqual( "forty-seven", leftleft.leaf() );
	}
	{
		node_ptr elem = tree.subtree("o", 0x1F); // don't match the final 5 bits
		assertTrue( elem.isNode() );
		node_ptr right = elem.node()->child[1];
		assertStringsEqual( "two", right.leaf() );

		node_ptr left = elem.node()->child[0];
		assertTrue(left.isNode());
		node_ptr leftright = left.node()->child[1];
		assertStringsEqual( "one", leftright.leaf() );
	}
	{
		// tree root
		node_ptr elem = tree.subtree("", 0xFF);
		assertTrue( elem.isNode() );
		node_ptr right = elem.node()->child[1];
		assertStringsEqual( "two", right.leaf() );
	}
	{
		// tree root again
		node_ptr elem = tree.subtree("whocares", 0xFF, 1);
		assertTrue( elem.isNode() );
		node_ptr right = elem.node()->child[1];
		assertStringsEqual( "two", right.leaf() );
	}
	{
		node_ptr elem = tree.subtree("forty");
		assertTrue( elem.isNode() );
		node_ptr left = elem.node()->child[0];
		assertStringsEqual( "forty-seven", left.leaf() );
		node_ptr right = elem.node()->child[1];
		assertStringsEqual( "forty-two", right.leaf() );
	}
}

TEST_CASE( "critbit_treeTest/testPrefixLookup.Nonsense", "[unit]" )
{
	// because the world isn't always happy path

	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("two") );
	assertEquals(2, tree.insert("forty-two") );
	assertEquals(2, tree.insert("forty-seven") );
	assertEquals(2, tree.insert("one") );

	/*
	 * for review:
	 *   't' == 116 == 01110100
	 *   'f' == 102 == 01100110
	 *   'o' == 111 == 01101111
	 *
	 * So tree should look like this,
	 *  where 0:16 means we branch on the 5th (10000 == 16) bit of the 1st (0th) byte:
	 *
	 *           0:16
	 *         /      \
	 *       0:8      two
	 *      /    \
	 *    6:4    one
	 *   .....
	 */

	using node_ptr = critbit_node_ptr<char, critbit_node>;

	// nearest_subtree -- return the nearest node, even if it doesn't match
	{
		// nonsense lookup finds node.. that doesn't match.
		node_ptr elem = tree.nearest_subtree("A");
		assertTrue( elem.isNode() );
	}
	{
		// nonsense lookup finds nearest leaf
		node_ptr elem = tree.nearest_subtree("nothing");
		assertFalse( elem.isNull() );
		assertTrue( elem.isLeaf() );
		assertStringsEqual( "one", elem.leaf() );
	}

	{
		// nonsense lookup finds node.. that doesn't match.
		node_ptr elem = tree.subtree("A");
		assertTrue( elem.isNull() );
	}
	{
		// nonsense lookup finds leaf.. that doesn't match. Return null.
		node_ptr elem = tree.subtree("nothing");
		assertTrue( elem.isNull() );
	}
	{
		// ignore final two bits, find our match
		node_ptr elem = tree.subtree("n", 0x3);
		assertFalse( elem.isNull() );
		assertTrue( elem.isLeaf() );
		assertStringsEqual( "one", elem.leaf() );
	}
}

TEST_CASE( "critbit_treeTest/testBegin", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("one") );
	assertEquals(2, tree.insert("five") );
	assertEquals(2, tree.insert("one-hundred") );

	const char* leaf = tree.begin();
	assertStringsEqual( "five", leaf );

	using node_ptr = critbit_node_ptr<char, critbit_node>;
	node_ptr node = tree.subtree("o");
	assertTrue( node.isNode() );
	assertStringsEqual( "one", tree.begin(node) );
}

TEST_CASE( "critbit_treeTest/testEnumerate", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("two") );
	assertEquals(2, tree.insert("forty-two") );
	assertEquals(2, tree.insert("forty-seven") );
	assertEquals(2, tree.insert("one") );

	/*
	 * for review:
	 *   't' == 116 == 01110100
	 *   'f' == 102 == 01100110
	 *   'o' == 111 == 01101111
	 *   's' == 115 == 01110011
	 *
	 * So tree should look like this,
	 *  where 0:16 means we branch on the 5th (10000 == 16) bit of the 1st (0th) byte:
	 *
	 *           0:16
	 *         /      \
	 *       0:8      two
	 *      /    \
	 *    6:4    one
	 *   .....
	 */

	// various ways to iterate over leaves
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "one", "one");
		assertEquals( "one", StringUtil::stlJoin(words) );
	}
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "o", "one");
		assertEquals( "one", StringUtil::stlJoin(words) );
	}
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "one", "no");
		assertEquals( "one", StringUtil::stlJoin(words) );
	}

	// multiple elements
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "one", "two");
		assertEquals( "one two", StringUtil::stlJoin(words) );
	}

	// partial match
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "forty", "forty-two");
		assertEquals( "forty-seven forty-two", StringUtil::stlJoin(words) );
	}

	// no elements
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "no", "nope");
		tree.enumerate(fun, "no", "no");
		tree.enumerate(fun, "nope", "none");
		tree.enumerate(fun, "none", "nope");
		assertEquals( "", StringUtil::stlJoin(words) );
	}

	// inclusive
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "forty", "two");
		assertEquals( "forty-seven forty-two one two", StringUtil::stlJoin(words) );
	}

	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "forty-two", "one");
		assertEquals( "forty-two one", StringUtil::stlJoin(words) );
	}

	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "forty-seven", "forty-seven");
		assertEquals( "forty-seven", StringUtil::stlJoin(words) );
	}

	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		tree.enumerate(fun, "forty-seven", "forty-two");
		assertEquals( "forty-seven forty-two", StringUtil::stlJoin(words) );
	}
}

TEST_CASE( "critbit_treeTest/testEnumerate.FunctionSaysStop", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("two") );
	assertEquals(2, tree.insert("forty-two") );
	assertEquals(2, tree.insert("forty-seven") );
	assertEquals(2, tree.insert("one") );

	/*
	 * for review:
	 *   't' == 116 == 01110100
	 *   'f' == 102 == 01100110
	 *   'o' == 111 == 01101111
	 *   's' == 115 == 01110011
	 *
	 * So tree should look like this,
	 *  where 0:16 means we branch on the 5th (10000 == 16) bit of the 1st (0th) byte:
	 *
	 *           0:16
	 *         /      \
	 *       0:8      two
	 *      /    \
	 *    6:4    one
	 *   .....
	 */

	// limit amount of enumeration. "false" will terminate the operation.
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return words.size() < 2; };
		tree.enumerate(fun, "a", "z");
		assertEquals( "forty-seven forty-two", StringUtil::stlJoin(words) );
	}
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return words.size() < 3; };
		tree.enumerate(fun, "a", "z");
		assertEquals( "forty-seven forty-two one", StringUtil::stlJoin(words) );
	}
	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return false; };
		tree.enumerate(fun, "a", "z");
		assertEquals( "forty-seven", StringUtil::stlJoin(words) );
	}
}

TEST_CASE( "critbit_treeTest/testEnumerateNode", "[unit]" )
{
	critbit_tree<char, const char*> tree;

	assertEquals(2, tree.insert("two") );
	assertEquals(2, tree.insert("forty-two") );
	assertEquals(2, tree.insert("forty-seven") );
	assertEquals(2, tree.insert("one") );

	// the alternative version of enumerate, for those who know what they're doing
	using node_ptr = critbit_node_ptr<char, critbit_node>;
	node_ptr top = tree.subtree("forty");

	{
		std::vector<string> words;
		std::function<bool(const char*)> fun = [&](const char* word){ words.push_back(string(word)); return true; };
		assertTrue( tree.enumerate(fun, top) );
		assertEquals( "forty-seven forty-two", StringUtil::stlJoin(words) );
	}
}


namespace {
	class KeyString
	{
	public:
		KeyString(const string& str, const string& payload = "")
			: _str(str)
			, _payload(payload)
		{}

		operator const uint8_t*() const
		{
			return (const uint8_t*)_str.c_str();
		}

		bool operator==(const KeyString& right) const
		{
			return _str == right._str;
		}

		size_t key_size() const
		{
			return _str.size()+1;
		}

	public:
		string _str;
		string _payload;
	};
}

TEST_CASE( "critbit_treeTest/testClassKeyCstr", "[unit]" )
{
	critbit_tree<KeyString> tree;

	assertEquals( 2, tree.insert(KeyString("turtle", "banana")) );
	assertTrue( tree.contains(KeyString("turtle")) );
	assertEquals( "banana", tree.lower_bound(KeyString("turtle"))->_payload );

	assertEquals(2, tree.insert(KeyString("monkey", "orange")) );
	assertTrue( tree.contains(KeyString("monkey")) );
	assertEquals( "orange", tree.lower_bound(KeyString("monkey"))->_payload );

	assertEquals(2, tree.insert(KeyString("forty-seven", "rocketship")) );
	assertTrue( tree.contains(KeyString("forty-seven")) );
	assertEquals( "rocketship", tree.lower_bound(KeyString("forty-seven"))->_payload );

	assertEquals(1, tree.insert(KeyString("turtle")) );
	assertTrue( tree.contains(KeyString("turtle")) );
	assertEquals(1, tree.remove(KeyString("turtle")) );
	assertFalse( tree.contains(KeyString("turtle")) );

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

TEST_CASE( "critbit_treeTest/testClassKeyCstr_Load", "[unit]" )
{
	critbit_tree<KeyString> tree;

	for (int i = 1000; i > 0; --i)
	{
		std::stringstream ss;
		ss << i;
		assertEquals( 2, tree.insert(KeyString(ss.str(), "banana")) );
		KeyString* internalNode = tree.lower_bound(ss.str());
		assertNotNull(internalNode);
		assertEquals( ss.str(), internalNode->_str );
		assertEquals( "banana", internalNode->_payload );
	}
}

TEST_CASE( "critbit_treeTest/testClassKeyCstr_Random", "[unit]" )
{
	critbit_tree<KeyString> tree;

	std::vector<string> words = {"fee", "fi", "fo", "fum", "I", "smell", "the", "blood", "of", "an", "englishman"};

	while (!words.empty())
	{
		std::vector<string>::iterator it = Random::select(words.begin(), words.end(), words.size());
		if (tree.insert(KeyString(*it, *it)) == 2)
		{
			std::cout << *it << "  ";
			words.erase(it);
		}
	}
	std::cout << std::endl;

	for (std::vector<string>::const_iterator it = words.begin(); it != words.end(); ++it)
	{
		KeyString* internalNode = tree.lower_bound(*it);
		assertNotNull(internalNode);
		assertEquals(*it, internalNode->_payload);
	}
}

namespace {
	class KeyInt
	{
	public:
		KeyInt(unsigned key, const string& payload = "")
			: _key(key)
			, _payload(payload)
		{}

		operator const uint8_t*() const
		{
			return (const uint8_t*)&_key;
		}

		bool operator==(const KeyInt& right) const
		{
			return _key == right._key;
		}

		size_t key_size() const
		{
			return sizeof(unsigned);
		}

	public:
		unsigned _key;
		string _payload;
	};
}

TEST_CASE( "critbit_treeTest/testClassKeyInt", "[unit]" )
{
	critbit_tree<KeyInt> tree;

	assertEquals( 2, tree.insert(KeyInt(10, "banana")) );
	assertTrue( tree.contains(10) );
	assertEquals( "banana", tree.lower_bound(10)->_payload );

	assertEquals(2, tree.insert(KeyInt(20, "orange")) );
	assertTrue( tree.contains(20) );
	assertEquals( "orange", tree.lower_bound(20)->_payload );

	assertEquals(2, tree.insert(KeyInt(47, "rocketship")) );
	assertTrue( tree.contains(47) );
	assertEquals( "rocketship", tree.lower_bound(47)->_payload );

	assertEquals(1, tree.insert(10) );
	assertTrue( tree.contains(10) );
	assertEquals(1, tree.remove(10) );
	assertFalse( tree.contains(10) );

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

TEST_CASE( "critbit_treeTest/testClassKeyInt_Load", "[unit]" )
{
	critbit_tree<KeyInt> tree;

	for (int i = 0xFF; i > 0; --i)
	{
		assertEquals( 2, tree.insert(KeyInt(i, "banana")) );

		KeyInt* internalNode = tree.lower_bound(i);
		assertNotNull( internalNode );
		assertEquals( i, internalNode->_key );
		assertEquals( "banana", internalNode->_payload );
	}

	KeyInt* node = tree.lower_bound(10);
	assertNotNull( node );
	assertEquals( 10, node->_key );
	assertEquals( "banana", node->_payload );
}
