#include "unittest.h"

#include "StringUtil.h"

#include "msgpack.hpp"
#include <string>
#include <vector>
using std::string;

namespace {
	class BaseClass
	{
	public:
		virtual ~BaseClass() {}

		virtual void msgpack_unpack(msgpack::object) = 0;
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
	std::vector<std::string> target;
	target.push_back("Hello,");
	target.push_back("World!");

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, target);

	// Deserialize
	msgpack::unpacked msg;
	msgpack::unpack(&msg, sbuf.data(), sbuf.size());
	std::vector<std::string> result;
	msg.get().convert(&result);

	assertEquals( "Hello, World!", StringUtil::join(result) );
}

TEST_CASE( "MessagePackTest/testClass", "[unit]" )
{
	CoolClass cool("foo", 1.5, {1,2,3});

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, cool);

	// Deserialize
	msgpack::unpacked msg;
	msgpack::unpack(&msg, sbuf.data(), sbuf.size());
	CoolClass result;
	msg.get().convert(&result);

	assertEquals( "foo", result.foo() );
	assertEquals( 1.5, result.bar() );
	assertEquals( "1 2 3", StringUtil::join(result.victor()) );
}

TEST_CASE( "MessagePackTest/testDeserialzeBaseClas", "[unit]" )
{
	CoolClass cool("foo", 1.5, {1,2,3});

	// Serialize
	msgpack::sbuffer sbuf;
	msgpack::pack(&sbuf, cool);

	// Deserialize
	msgpack::unpacked msg;
	msgpack::unpack(&msg, sbuf.data(), sbuf.size());
	CoolClass result;

	BaseClass* base(&result);
	msg.get().convert(base);

	assertEquals( "foo", result.foo() );
	assertEquals( 1.5, result.bar() );
	assertEquals( "1 2 3", StringUtil::join(result.victor()) );
}

