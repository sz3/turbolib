#include "catch.hpp"

#include "CommandLine.h"
using std::string;

TEST_CASE( "CommandLineTest/testRun", "description" )
{
	string result = CommandLine::run("ls");
	REQUIRE( result == "CMakeFiles\n" );
}

