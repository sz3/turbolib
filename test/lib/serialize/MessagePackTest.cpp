/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "serialize/str_join.h"

#include "msgpack.hpp"
#include <string>
#include <vector>
using std::string;

namespace {
	class BaseClass
	{
	public:
		virtual ~BaseClass() {}

		virtual void msgpack_unpack(msgpack::object const&) = 0;
	};

	class CoolClass : public BaseClass
	{
	public:
		CoolClass()
			: _bar(0)
		{}

		CoolClass(string foo, double bar, std::vector<int> victor)
			: _foo(foo)
			, _bar(bar)
			, _victor(victor)
		{}

		string foo() const
		{
			return _foo;
		}

		double bar() const
		{
			return _bar;
		}

		std::vector<int> victor() const
		{
			return _victor;
		}

	protected:
		string _foo;
		double _bar;
		std::vector<int> _victor;

	public:
		MSGPACK_DEFINE(_foo, _bar, _victor);
	};
}

TEST_CASE( "MessagePackTest/testDefault", "[unit]" )
{
	std::vector<std::string> datums;
	datums.push_back("Hello,");
	datums.push_back("World!");

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, datums);

	// Deserialize
	msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
	std::vector<std::string> result;
	oh.get().convert(result);

	assertEquals( "Hello, World!", turbo::str::join(result) );
}

TEST_CASE( "MessagePackTest/testClass", "[unit]" )
{
	CoolClass cool("foo", 1.5, {1,2,3});

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, cool);

	// Deserialize
	msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
	CoolClass result;
	oh.get().convert(result);

	assertEquals( "foo", result.foo() );
	assertEquals( 1.5, result.bar() );
	assertEquals( "1 2 3", turbo::str::join(result.victor()) );
}

TEST_CASE( "MessagePackTest/testDeserialzeBaseClass", "[unit]" )
{
	CoolClass cool("foo", 1.5, {1,2,3});

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, cool);

	// Deserialize
	msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
	CoolClass result;

	BaseClass* base(&result);
	oh.get().convert(*base);

	assertEquals( "foo", result.foo() );
	assertEquals( 1.5, result.bar() );
	assertEquals( "1 2 3", turbo::str::join(result.victor()) );
}

