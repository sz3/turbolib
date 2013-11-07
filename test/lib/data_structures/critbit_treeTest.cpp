#include "unittest.h"

#include "critbit_tree.h"
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
	std::cout << "find? " << (unsigned)tree.find("one") << std::endl;
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

		static size_t key_size(const uint8_t* cstr)
		{
			return strlen((const char*)cstr);
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
	assertEquals( "banana", tree.find(KeyString("turtle"))->_payload );

	assertEquals(2, tree.insert(KeyString("monkey", "orange")) );
	assertTrue( tree.contains(KeyString("monkey")) );
	assertEquals( "orange", tree.find(KeyString("monkey"))->_payload );

	assertEquals(2, tree.insert(KeyString("forty-seven", "rocketship")) );
	assertTrue( tree.contains(KeyString("forty-seven")) );
	assertEquals( "rocketship", tree.find(KeyString("forty-seven"))->_payload );

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
		KeyString* internalNode = tree.find(ss.str());
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
		KeyString* internalNode = tree.find(*it);
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

		static size_t key_size(const uint8_t*)
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
	assertEquals( "banana", tree.find(10)->_payload );

	assertEquals(2, tree.insert(KeyInt(20, "orange")) );
	assertTrue( tree.contains(20) );
	assertEquals( "orange", tree.find(20)->_payload );

	assertEquals(2, tree.insert(KeyInt(47, "rocketship")) );
	assertTrue( tree.contains(47) );
	assertEquals( "rocketship", tree.find(47)->_payload );

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

		KeyInt* internalNode = tree.find(i);
		assertNotNull( internalNode );
		assertEquals( i, internalNode->_key );
		assertEquals( "banana", internalNode->_payload );
	}

	KeyInt* node = tree.find(10);
	assertNotNull( node );
	assertEquals( 10, node->_key );
	assertEquals( "banana", node->_payload );
}
