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

TEST_CASE( "SimplePoolTest/testInsert", "[test]" )
{
	TestableSimplePool pool;
	assertEquals( 0, pool._connections.size() );

	assertTrue( !!pool.insert(udp_socket(socket_address("10.1.2.3:2"))) );
	assertEquals( 1, pool._connections.size() );

	{
		shared_ptr<ISocketWriter> writer;
		assertFalse( pool.insert(udp_socket(socket_address("10.1.2.3:2")), writer) );
		assertEquals( 1, pool._connections.size() );
		assertTrue( !!writer );
	}

	{
		shared_ptr<ISocketWriter> writer;
		assertFalse( pool.insert(udp_socket(socket_address("10.1.2.3:2")), writer) );
		assertEquals( 1, pool._connections.size() );
		assertTrue( !!writer );
	}
}
