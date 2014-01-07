/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "catch.hpp"

#include "CommandLine.h"
using std::string;

TEST_CASE( "CommandLineTest/testRun", "description" )
{
	string result = CommandLine::run("ls | grep cmake");
	REQUIRE( "cmake_install.cmake\n"
			 "CTestTestfile.cmake\n" == result );
}

