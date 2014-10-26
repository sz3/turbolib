/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "SimplePool.h"

#include "socket_address.h"
#include "udp_socket.h"
using std::shared_ptr;

namespace {
	class TestableSimplePool : public SimplePool<udp_socket>
	{
	public:
		using SimplePool<udp_socket>::_connections;
	};
}

TEST_CASE( "SimplePoolTest/testAdd", "[test]" )
{
	TestableSimplePool pool;
	assertEquals( 0, pool._connections.size() );

	assertTrue( pool.add(udp_socket(socket_address("10.1.2.3:2"))) );
	assertEquals( 1, pool._connections.size() );

	assertFalse( pool.add(udp_socket(socket_address("10.1.2.3:2"))) );
	assertEquals( 1, pool._connections.size() );

	shared_ptr<ISocketWriter> writer;
	assertFalse( pool.add(udp_socket(socket_address("10.1.2.3:2")), writer) );
	assertEquals( 1, pool._connections.size() );
	assertTrue( !!writer );
}
