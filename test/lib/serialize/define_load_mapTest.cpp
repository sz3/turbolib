#include "unittest.h"

#include "define_load_map.h"

namespace {
	class FooClass
	{
	public:
		int foo;
		DEFINE_LOAD_MAP(foo);
	};
}

TEST_CASE( "define_load_mapTest/testDefault", "description" )
{
	std::unordered_map<std::string,std::string> vals;
	vals["foo"] = "5";

	FooClass foo;
	foo.load(vals);
	assertEquals(5, foo.foo);
}

namespace {
	class BarClass
	{
	public:
		int foo;
		double bar;
		std::string welp;
		DEFINE_LOAD_MAP(foo, bar, welp);
	};
}

TEST_CASE( "define_load_mapTest/testRicherObject", "description" )
{
	std::unordered_map<std::string,std::string> vals;
	vals["foo"] = "55";
	vals["bar"] = "5.5";
	vals["welp"] = "onoes";

	BarClass bar;
	bar.load(vals);
	assertEquals(55, bar.foo);
	assertEquals(5.5, bar.bar);
	assertEquals("onoes", bar.welp);
}

