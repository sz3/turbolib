#include "unittest.h"

#include "critbit_tree.h"
#include <iostream>
#include <string>
using std::string;

namespace {
	class Foo
	{
	public:
		Foo(const string& str, const string& payload = "")
			: _str(str)
			, _payload(payload)
		{}

		operator const uint8_t*() const
		{
			return (const uint8_t*)_str.c_str();
		}

	public:
		string _str;
		string _payload;
	};
}

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

TEST_CASE( "critbit_treeTest/testClass", "[unit]" )
{
	critbit_tree<Foo> tree;

	Foo turtle("turtle", "banana");
	assertEquals( 2, tree.insert(turtle) );
	assertTrue( tree.contains(Foo("turtle")) );
	assertEquals( "banana", tree.find(Foo("turtle"))->_payload );

	Foo monkey("monkey", "orange");
	assertEquals(2, tree.insert(monkey) );
	assertTrue( tree.contains(Foo("monkey")) );
	assertEquals( "orange", tree.find(Foo("monkey"))->_payload );

	Foo fortyseven("forty-seven", "rocketship");
	assertEquals(2, tree.insert(fortyseven) );
	assertTrue( tree.contains(Foo("forty-seven")) );
	assertEquals( "rocketship", tree.find(Foo("forty-seven"))->_payload );

	assertEquals(1, tree.insert(Foo("turtle")) );
	assertTrue( tree.contains(Foo("turtle")) );

	assertFalse(tree.empty());
	tree.clear();
	assertTrue(tree.empty());
}

