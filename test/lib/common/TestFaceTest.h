#pragma once
#include "catch.hpp"

#include "TestFace.h"

TEST_CASE( "TestFaceTest_testDefault", "default" )
{
	TestFace face;
	REQUIRE( face.method() == 3 );
}
