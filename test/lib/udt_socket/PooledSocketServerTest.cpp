/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "udt_socket.h"
#include "udt_socket_set.h"
#include "UdtScope.h"
#include "socket/PooledSocketServer.h"

#include "concurrent/monitor.h"
#include "socket/ISocketWriter.h"
#include "socket/socket_address.h"
#include "time/wait_for.h"
#include "util/CallHistory.h"
#include <condition_variable>
#include <memory>
using namespace std;
using namespace std::placeholders;
using turbo::monitor;
using turbo::str::str;

namespace {
	class PacketHandler
	{
	public:
		std::function<void(ISocketWriter&, const char*, unsigned)> readFun()
		{
			return bind(&PacketHandler::onRead, this, _1, _2, _3);
		}

		std::function<bool(int)> writeFun()
		{
			return bind(&PacketHandler::onWriteReady, this, _1);
		}

		std::string calls() const
		{
			return _history.calls();
		}

		const socket_address& lastAddr() const
		{
			return _lastAddr;
		}

	protected:
		void onRead(ISocketWriter& writer, const char* buff, unsigned size)
		{
			_history.call("onRead", string(buff, size));
			_lastAddr = writer.endpoint();
			writer.send(buff, size);
		}

		bool onWriteReady(int id)
		{
			_history.call("onWriteReady", id);
			return true;
		}

	protected:
		CallHistory _history;
		socket_address _lastAddr;
	};
}

TEST_CASE( "PooledSocketServerTest/testAcceptAndReply", "[unit]" )
{
	UdtScope udt;

	PacketHandler handler;
	PooledSocketServer<udt_socket, udt_socket_set> server(socket_address("", 8487), handler.readFun(), handler.writeFun());
	assertMsg( server.start(), server.lastError() );

	udt_socket client(socket_address("127.0.0.1", 8487));
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

	assertEquals( "onRead(hello, darkness)|onRead(my old friend)", handler.calls() );
}


TEST_CASE( "PooledSocketServerTest/testNoContact", "[unit]" )
{
	UdtScope udt;

	PacketHandler handler;
	PooledSocketServer<udt_socket, udt_socket_set> server(socket_address("", 8487), handler.readFun(), handler.writeFun());
	assertMsg( server.start(), server.lastError() );

	server.stop();
	server.stop();
}

TEST_CASE( "PooledSocketServerTest/testServerCrosstalk", "[unit]" )
{
	UdtScope udt;

	PacketHandler handler;
	PooledSocketServer<udt_socket, udt_socket_set> server(socket_address("", 8487), handler.readFun(), handler.writeFun());
	assertMsg( server.start(), server.lastError() );

	PooledSocketServer<udt_socket, udt_socket_set> other(socket_address("", 8488), handler.readFun(), handler.writeFun());
	assertMsg( other.start(), other.lastError() );

	{
		shared_ptr<ISocketWriter> writer( other.getWriter(socket_address("127.0.0.1", 8487)) );
		assertNotNull( writer.get() );

		string request = "hi world";
		assertEquals( 8, writer->send(request.data(), request.size()) );

		string response;
		response.resize(100);

		SocketWriter<udt_socket>* sock = (SocketWriter<udt_socket>*)writer.get();
		assertEquals( 8, sock->recv(&response[0], response.size()) );
		assertEquals( "hi world", string(response.data(), 8) );
		assertEquals( "onRead(hi world)", handler.calls() );
	}

	// and then the other way

	{
		shared_ptr<ISocketWriter> writer( server.getWriter(handler.lastAddr()) );
		assertNotNull( writer.get() );

		string request = "bye world";
		assertEquals( 9, writer->send(request.data(), request.size()) );

		string response;
		response.resize(100);

		SocketWriter<udt_socket>* sock = static_cast<SocketWriter<udt_socket>*>(writer.get());
		assertEquals( 9, sock->recv(&response[0], response.size()) );
		assertEquals( "bye world", string(response.data(), 9) );
		assertEquals( "onRead(hi world)|onRead(bye world)", handler.calls() );

		assertEquals( "127.0.0.1:8487", handler.lastAddr().toString() );
	}
}

TEST_CASE( "PooledSocketServerTest/testLargeBlockingSend", "[unit]" )
{
	UdtScope udt;

	unsigned bytesRecv = 0;
	auto readFun = [&bytesRecv] (ISocketWriter& writer, const char* buff, unsigned size) { bytesRecv += size; };
	auto writeFun = [] (int) { return true; };

	PooledSocketServer<udt_socket, udt_socket_set> server(socket_address("", 8487), readFun, writeFun);
	assertMsg( server.start(), server.lastError() );

	udt_socket client(socket_address("127.0.0.1", 8487));
	assertTrue( client.good() );

	string packet = "01234567890123456789012345678901234567890123456789abcdef";
	for (int i = 0; i < 100000; ++i)
		assertEquals( 56, client.send(packet.data(), packet.size()) );

	unsigned expected = 56*100000;
	wait_for_equal(2, expected, [&]()
	{
		return bytesRecv;
	});
}

TEST_CASE( "PooledSocketServerTest/testNonBlockingSend", "[unit]" )
{
	UdtScope udt;

	monitor readyForWrite;
	unsigned bytesRecv = 0;
	auto readFun = [&bytesRecv] (ISocketWriter& writer, const char* buff, unsigned size) { bytesRecv += size; };
	auto writeFun = [&readyForWrite] (int) { readyForWrite.signal_all(); return true; };

	PooledSocketServer<udt_socket, udt_socket_set> server(socket_address("", 8487), readFun, writeFun);
	assertMsg( server.start(), server.lastError() );

	shared_ptr<ISocketWriter> client = server.getWriter(socket_address("127.0.0.1", 8487));
	assertTrue( !!client );

	string packet = "01234567890123456789012345678901234567890123456789abcdef";
	int i = 0;
	while (i < 100000)
	{
		int res = client->send(packet.data(), packet.size());
		if (res != 56)
		{
			std::cout << " write(" << i << ") failed, res " << res << std::endl;
			server.waitForWriter(client->handle());
			readyForWrite.wait_for(5000);
		}
		else
			++i;
	}

	unsigned expected = 56*100000;
	wait_for_equal(2, expected, [&]()
	{
		return bytesRecv;
	});
}
