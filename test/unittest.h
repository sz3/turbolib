#include "catch.hpp"

#define assertTrue(expr) REQUIRE(expr)
#define assertFalse(expr) REQUIRE_FALSE(expr)
#define assertEquals(x,y) REQUIRE(x == y)
#define assertMsg(expr,msg) bool res = expr; if(!res) FAIL(msg); REQUIRE(res)
