#include "unittest.h"

#include "merkle_tree.h"
#include <iostream>

TEST_CASE( "merkle_treeTest/testDefault", "[unit]" )
{
	merkle_tree<unsigned, unsigned> merk;
	merk.set(8, 0x8);
	merk.set(4, 0x4);
	merk.set(2, 0x2);
	merk.set(1, 0x1);
	merk.set(65, 65);
	merk.set(0xFF, 0xFF);

	merk.print();
}

