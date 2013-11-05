#include "unittest.h"

#include "critbit_tree.h"
#include <iostream>

TEST_CASE( "critbit_treeTest/testInsertLookup", "[unit]" )
{
	critbit_tree<char> tree;

	std::cout << "insert? " << tree.insert("one") << std::endl;
	std::cout << "find? " << (unsigned)tree.find("one") << std::endl;
	std::cout << "contains? " << tree.contains("one") << std::endl;

	assertEquals(2, tree.insert("two") );
	assertTrue( tree.contains("two") );

	assertEquals(2, tree.insert("forty-seven") );
	assertTrue( tree.contains("forty-seven") );

	assertEquals(1, tree.insert("one") );
	assertTrue( tree.contains("one") );
}

