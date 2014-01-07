/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "File.h"
#include "FileRemover.h"
using std::string;

TEST_CASE( "FileTest/testReadWrite", "default" )
{
	FileRemover remover("foo.test");
	assertTrue( File::save("foo.test", "I am a file") );

	string contents;
	assertTrue( File::load("foo.test", contents) );
	assertEquals( "I am a file", contents );
}

TEST_CASE( "FileTest/testMultilineReadWrite", "default" )
{
	FileRemover remover("foo.test");
	assertTrue( File::save("foo.test", "I am a file\nWith TWO lines!") );

	string contents;
	assertTrue( File::load("foo.test", contents) );
	assertEquals( "I am a file\nWith TWO lines!", contents );
}

