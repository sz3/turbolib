/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "serialize/define_serialize_map.h"

namespace {
	class FooClass
	{
	public:
		int foo;
		DEFINE_SERIALIZE_MAP(foo);
	};
}

TEST_CASE( "define_serialize_mapTest/testLoad", "[unit]" )
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
		DEFINE_SERIALIZE_MAP(foo, bar, welp);
	};
}

TEST_CASE( "define_serialize_mapTest/testRicherLoad", "[unit]" )
{
	std::unordered_map<std::string,std::string> vals;
	vals["foo"] = "55";
	vals["bar"] = "5.5";
	vals["welp"] = "onoes this has spaces!";

	BarClass bar;
	bar.load(vals);
	assertEquals(55, bar.foo);
	assertEquals(5.5, bar.bar);
	assertEquals("onoes this has spaces!", bar.welp);
}

TEST_CASE( "define_serialize_mapTest/testSave", "[unit]" )
{
	BarClass bar;
	bar.foo = 55;
	bar.bar = 5.5;
	bar.welp = "welp";

	std::unordered_map<std::string,std::string> vals;
	bar.save(vals);
	assertEquals("55", vals["foo"]);
	assertEquals("5.5", vals["bar"]);
	assertEquals("welp", vals["welp"]);
}
