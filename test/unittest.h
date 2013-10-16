#include "catch.hpp"

#define assertTrue(expr) REQUIRE(res)
#define assertMsg(expr,msg) bool res = expr; if(!res) FAIL(msg); REQUIRE(res)
