/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "merkle_experiment.h"
#include "serialize/StringUtil.h"
#include <bitset>
#include <iostream>

// two parties: initiator (I) and responder (R)
// I: ping|hash=0xf00
// R: merkle_resp|level=0|[1024 bytes]
// I: merkle_req|level=1 prefix=1337|
// R: merkle_resp|level=1 prefix=1337|[1024 bytes]
// I: file_req|name=foo|

namespace {
	template <typename ValueType>
	std::vector<unsigned> diff(const::std::vector<ValueType>& left, const std::vector<ValueType>& right)
	{
		std::vector<unsigned> diffs;
		unsigned size = std::min(left.size(), right.size());
		for (unsigned i = 0; i < size; ++i)
			if (left[i] != right[i])
				diffs.push_back(i);
		return diffs;
	}
}

TEST_CASE( "merkle_experimentTest/testInsertRemove", "[unit]" )
{
	merkle_tree<unsigned, unsigned> merk;
	assertEquals( 0, merk.count() );

	assertEquals( 16, merk.insert(16, 0x10) );
	assertEquals( 1, merk.count() );
	assertEquals( 0, merk.remove(16) );
	assertEquals( 0, merk.count() );

	assertEquals( 8, merk.insert(8, 0x8) );
	assertEquals( 8, merk.insert(8, 0x8) );
	assertEquals( 8, merk.insert(8, 0x8) );
	assertEquals( 12, merk.insert(4, 0x4) );
	assertEquals( 14, merk.insert(2, 0x2) );
	assertEquals( 15, merk.insert(1, 0x1) );
	merk.insert(0xFF, 0xFF);

	assertEquals( 5, merk.count() );
	assertEquals( 0xF0, merk.value() );

	merk.insert(65, 65); // same box as '1'!
	assertEquals( 5, merk.count() );
	assertEquals( 177, merk.value() );

	merk.print();

	assertEquals( "0 64 2 0 4 0 0 0 8 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 255", StringUtil::join(merk.values()) );

	const merkle_tree<unsigned, unsigned>* levelOneOne = merk.subtree(1, 1);
	assertNotNull( levelOneOne );
	assertEquals( "1 65 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0", StringUtil::join(levelOneOne->values()) );

	assertNull( merk.subtree(0) );
	assertNull( merk.subtree(2) );
	for (int i = 3; i < 64; ++i)
		assertNull( merk.subtree(i) );

	assertEquals( 0xF0, merk.remove(65) );

	assertEquals(  "0 1 2 0 4 0 0 0 8 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
				  " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 255", StringUtil::join(merk.values()) );
	assertNull( merk.subtree(1) );
}

TEST_CASE( "merkle_experimentTest/testRecursion", "[unit]" )
{
	merkle_tree<unsigned, unsigned> merk;
	assertEquals( 0, merk.count() );

	assertEquals( 1, merk.insert(1, 0x1) );
	assertEquals( 4096, merk.insert(4097, 0x1001) ); // same box as '1'!
	assertEquals( 1, merk.count() );

	merk.print();

	// what do you do when you traverse multiple levels? (e.g, sparse density...)
	// a naive, structured approach clearly doesn't work

	assertEquals( 1, merk.remove(4097) );
	for (int i = 0; i < 64; ++i)
		assertNull( merk.subtree(i) );
}

TEST_CASE( "merkle_experimentTest/testBigValue", "[unit]" )
{
	merkle_tree<unsigned long long, std::bitset<128> > merk;
	assertEquals( 0, merk.count() );

	assertEquals( "000000000000000000000000000000000000000000000000000000000000000000000000000"
				  "00000000000000000000000000000000000000000000000000001", merk.insert(1, 0x1).to_string() );
	assertEquals( "000000000000000000000000000000000000000000000000000000000000000000000000000"
				  "00000000000000000000000000000000000000001000000000000", merk.insert(4097, 0x1001).to_string() ); // same box as '1'!
	assertEquals( 1, merk.count() );

	assertEquals( "000000000000000000000000000000000000000000000000000000000000000000000000000"
				  "00000000000000000000000000000000000000000000000000001", merk.remove(4097).to_string() );
	for (int i = 0; i < 64; ++i)
		assertNull( merk.subtree(i) );
}


TEST_CASE( "merkle_experimentTest/testLoad", "[unit]" )
{
	merkle_tree<unsigned long long, std::bitset<128> > merk;
	assertEquals( 0, merk.count() );

	for (unsigned i = 0; i < 100000; ++i)
		merk.insert(i, i);
	assertEquals( 64, merk.count() );
}

