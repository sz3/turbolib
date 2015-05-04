/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StatelessSocketServer.h"

#include "socket_address.h"
#include "udp_socket.h"
#include "system/popen.h"
#include "util/CallHistory.h"
using namespace std;
using namespace std::placeholders;

namespace {
	class PacketHandler
	{
	public:
		void onRead(ISocketWriter& writer, const char* buff, unsigned size)
		{
			_history.call("onRead", string(buff, size));
			writer.send(buff, size);
		}

		std::string calls()
		{
			return _history.calls();
		}

	protected:
		CallHistory _history;
	};
}

TEST_CASE( "StatelessSocketServerTest/testDefault", "default" )
{
	PacketHandler handler;
	StatelessSocketServer<udp_socket> server(socket_address("", 8487), bind(&PacketHandler::onRead, ref(handler), _1, _2, _3));
	assertMsg( server.start(), server.lastError() );

	udp_socket client(socket_address("127.0.0.1", 8487));
	assertTrue( client.good() );

	assertEquals( 15, client.send("hello, darkness") );
	std::string buff;
	buff.resize(100);
	assertEquals( 15, client.recv(&buff[0], buff.size()) );
	assertEquals( "hello, darkness", string(buff.data(), 15) );

	assertEquals( 13, client.send("my old friend") );
	assertEquals( 13, client.recv(&buff[0], buff.size()) );
	assertEquals( "my old friend", string(buff.data(), 13) );
	server.stop();

	// sends, but there's no server no more...
	assertEquals( 4, client.send("welp") );
	assertEquals( "onRead(hello, darkness)|onRead(my old friend)", handler.calls() );
}

TEST_CASE( "StatelessSocketServerTest/testSendFromOtherProcess", "default" )
{
	PacketHandler handler;
	StatelessSocketServer<udp_socket> server(socket_address("", 8487), bind(&PacketHandler::onRead, ref(handler), _1, _2, _3));
	assertMsg( server.start(), server.lastError() );

	// -w 0 option makes nc exit immediately.
	turbo::popen("echo 'hello' | nc -u 127.0.0.1 8487 -w 0");
	assertEquals( "onRead(hello\n)", handler.calls() );
}
