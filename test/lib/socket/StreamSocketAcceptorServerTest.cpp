/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "StreamSocketAcceptorServer.h"

#include "local_stream_socket.h"
#include "socket_address.h"
#include "system/popen.h"
#include "util/CallHistory.h"
using namespace std;
using namespace std::placeholders;

namespace {
	class PacketHandler
	{
	public:
		void onConnect(local_stream_socket sock)
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
			sock.close();
		}

		std::string calls()
		{
			return _history.calls();
		}

	protected:
		CallHistory _history;
	};
}

TEST_CASE( "StreamSocketAcceptorServerTest/testEcho", "[unit]" )
{
	PacketHandler handler;
	StreamSocketAcceptorServer<local_stream_socket> server(socket_address("/tmp/testdomainsocketserver"), bind(&PacketHandler::onConnect, ref(handler), _1));
	assertMsg( server.start(), server.lastError() );

	{
		string response = turbo::popen("echo 'hello' | nc -U /tmp/testdomainsocketserver").read();
		assertEquals( "onConnect()|recv(hello\n)", handler.calls() );
		assertEquals( "hello\n", response );
	}

	{
		string response = turbo::popen("echo 'again' | nc -U /tmp/testdomainsocketserver").read();
		assertEquals( "onConnect()|recv(hello\n)|onConnect()|recv(again\n)", handler.calls() );
		assertEquals( "again\n", response );
	}
}

