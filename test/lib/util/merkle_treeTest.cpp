#include "unittest.h"

#include "merkle_tree.h"
#include <iostream>

// two parties: initiator (I) and responder (R)
// I: ping|hash=0xf00
// R: merkle_resp|level=0|[1024 bytes]
// I: merkle_req|level=1 prefix=1337|
// R: merkle_resp|level=1 prefix=1337|[1024 bytes]
// I: file_req|name=foo|

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

