#include "catch.hpp"

#define assertTrue(expr) REQUIRE(expr)
#define assertFalse(expr) REQUIRE_FALSE(expr)
#define assertEquals(x,y) REQUIRE(x == y)
#define assertStringsEqual(x,y) REQUIRE(std::string(x) == std::string(y))
#define assertMsg(expr,msg) bool res = expr; if(!res) FAIL(msg); REQUIRE(res)
#define assertInRange(x,y,z) REQUIRE(x <= z); REQUIRE(z <= y)
#define assertNull(expr) REQUIRE(expr == (void*)NULL)
#define assertNotNull(expr) REQUIRE(expr != (void*)NULL)
