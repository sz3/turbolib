#include "catch.hpp"

#include "CommandLine.h"
using std::string;

TEST_CASE( "CommandLineTest/testRun", "description" )
{
	string result = CommandLine::run("ls | grep cmake");
	REQUIRE( "cmake_install.cmake\n"
			 "CTestTestfile.cmake\n" == result );
}

