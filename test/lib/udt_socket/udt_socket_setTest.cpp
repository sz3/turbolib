/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "udt_socket_set.h"
#include "udt_socket.h"

#include "UdtScope.h"
#include "UdtServer.h"
#include "socket/socket_address.h"
using namespace std;

namespace {
	void onRead(ISocketWriter&, const char*, unsigned)
	{
	}

	bool onWriteReady(int)
	{
		return true;
	}
}

TEST_CASE( "udt_socket_setTest/testWriteWait", "default" )
{
	udt_socket_set epoll(udt_socket_set::WRITES);

	UdtScope udt;
	UdtServer server(socket_address("", 8487), &onRead, &onWriteReady);
	assertMsg( server.start(), server.lastError() );

	udt_socket one(socket_address("127.0.0.1", 8487));
	udt_socket two(socket_address("127.0.0.1", 8487));

	assertTrue( epoll.add(one.handle()) );
	assertTrue( epoll.add(two.handle()) );

	std::set<int> writers = epoll.wait();
	assertEquals( 2, writers.size() );
}

TEST_CASE( "udt_socket_setTest/testOverloadWriteWait", "default" )
{
	udt_socket_set epoll(udt_socket_set::WRITES);

	UdtScope udt;
	UdtServer server(socket_address("", 8487), &onRead, &onWriteReady);
	assertMsg( server.start(), server.lastError() );

	udt_socket client(socket_address("127.0.0.1", 8487));
	while (client.try_send("hello world", 11) == 11)
		; // write until failure

	assertTrue( epoll.add(client.handle()) );

	std::set<int> writers = epoll.wait();
	assertEquals( 1, writers.size() );

	assertEquals( 11, client.send("hello world", 11) );
	assertEquals( client.handle(), *writers.begin() );
}

