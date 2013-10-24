#include "unittest.h"

#include "ConfigFile.h"
#include "File.h"
#include "FileRemover.h"

TEST_CASE( "ConfigFileTest/testLoad", "default" )
{
	FileRemover remover("foo.config");
	assertTrue( File::save("foo.config", "foo=bar\nval=1\nx=y\ncool=true") );

	ConfigFile config("foo.config");
	assertTrue( config.load() );

	assertEquals( "bar", config.get("foo") );
	assertEquals( "1", config.get("val") );
	assertEquals( "y", config.get("x") );
	assertEquals( "true", config.get("cool") );

	assertTrue( File::remove("foo.config") );
	assertFalse( config.load() );
}

TEST_CASE( "ConfigFileTest/testSave", "default" )
{
	FileRemover remover("foo.config");

	ConfigFile config("foo.config");
	assertTrue( config.save() );

	std::string contents;
	assertTrue( File::load("foo.config", contents) );
	assertEquals( "", contents);

	config.set("john", "1");
	config.set("paul", "2");
	config.set("george", "3");
	config.set("ringo", "4");

	assertTrue( config.save() );
	assertTrue( File::load("foo.config", contents) );
	assertEquals( "george=3\njohn=1\npaul=2\nringo=4\n", contents );
}

