/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StreamSocketAcceptorServer.h"

#include "tcp_socket.h"
#include "socket_address.h"
#include "command_line/CommandLine.h"
#include "util/CallHistory.h"
using namespace std;
using namespace std::placeholders;

namespace {
	class PacketHandler
	{
	public:
		void onConnect(tcp_socket sock)
		{
			_history.call("onConnect");

			string buff;
			buff.resize(1024);

			int bytesRead = sock.recv(&buff[0], buff.size());
			if (bytesRead > 0)
			{
				_history.call("recv", string(buff.data(), bytesRead));
				sock.send(buff.data(), bytesRead);
			}
		}

		std::string calls()
		{
			return _history.calls();
		}

	protected:
		CallHistory _history;
	};
}

TEST_CASE( "TcpAcceptorServerTest/testLocalEcho", "[unit]" )
{
	PacketHandler handler;
	StreamSocketAcceptorServer<tcp_socket> server(socket_address("local", 9010), bind(&PacketHandler::onConnect, ref(handler), _1));
	assertMsg( server.start(), server.lastError() );

	{
		string response = CommandLine::run("echo 'hello' | nc localhost 9010");
		assertEquals( "onConnect()|recv(hello\n)", handler.calls() );
		assertEquals( "hello\n", response );
	}

	{
		string response = CommandLine::run("echo 'again' | nc localhost 9010");
		assertEquals( "onConnect()|recv(hello\n)|onConnect()|recv(again\n)", handler.calls() );
		assertEquals( "again\n", response );
	}
}

TEST_CASE( "TcpAcceptorServerTest/testAnyEcho", "[unit]" )
{
	PacketHandler handler;
	StreamSocketAcceptorServer<tcp_socket> server(socket_address("*", 9010), bind(&PacketHandler::onConnect, ref(handler), _1));
	assertMsg( server.start(), server.lastError() );

	{
		string response = CommandLine::run("echo 'hello' | nc localhost 9010");
		assertEquals( "onConnect()|recv(hello\n)", handler.calls() );
		assertEquals( "hello\n", response );
	}

	{
		string response = CommandLine::run("echo 'again' | nc localhost 9010");
		assertEquals( "onConnect()|recv(hello\n)|onConnect()|recv(again\n)", handler.calls() );
		assertEquals( "again\n", response );
	}
}

