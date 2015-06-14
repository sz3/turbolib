#include "catch.hpp"

#define assertMsg(expr,msg) {bool res = expr; if(!res) FAIL(msg); REQUIRE(res);}
#define assertTrue(expr) REQUIRE(expr)
#define assertFalse(expr) REQUIRE_FALSE(expr)
#define assertEquals(x,y) REQUIRE((x) == (y))
#define assertMatch(x,y) assertMsg(std::regex_match(y, std::regex(x)), x + std::string(" does not match ") + y)
#define assertStringsEqual(x,y) REQUIRE(std::string(x) == std::string(y))
#define assertStringContains(x,y) assertMsg(std::string(y).find(x) != std::string::npos, std::string(y) + " did not contain " + std::string(x))
#define assertInRange(x,y,z) REQUIRE(x <= y); REQUIRE(y <= z)
#define assertNull(expr) REQUIRE(expr == (void*)NULL)
#define assertNotNull(expr) REQUIRE(expr != (void*)NULL)
