/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "UdtEpoll.h"

#include "UdtScope.h"
#include "UdtServer.h"
#include "UdtSocket.h"
#include "serialize/StringUtil.h"
#include "socket/IpAddress.h"
#include "util/CallHistory.h"

#include "udt/udt.h"
using namespace std;

namespace {
	Event _packetEvent;
	IpAddress _serverLastAddr;
	CallHistory _writeReadyHistory;

	void onPacket(const IIpSocket& sock, const std::string& buffer)
	{
		_packetEvent.signal();
		_serverLastAddr = sock.getTarget();
	}

	void onWriteReady(const IIpSocket& sock)
	{
		_writeReadyHistory.call("onWriteReady", sock.getTarget().toString());
	}
}

TEST_CASE( "UdtEpollTest/testWait", "default" )
{
	_writeReadyHistory.clear();

	UdtScope udt;
	UdtServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	shared_ptr<IIpSocket> clientSock = server.sock(IpAddress("127.0.0.1", 8487));
	assertFalse( !clientSock );
	assertEquals( 11, clientSock->send("hello world", 11) );

	assertTrue( _packetEvent.wait() );

	shared_ptr<IIpSocket> servSock = server.sock(_serverLastAddr);
	assertFalse( !servSock );

	UdtEpoll epoll(&onWriteReady);
	assertTrue( epoll.add(*clientSock) );
	assertTrue( epoll.add(*servSock) );

	assertTrue( epoll.wait_clear() );

	vector<string> expected = {"onWriteReady("+_serverLastAddr.toString()+")", "onWriteReady(127.0.0.1:8487)"};
	sort(expected.begin(), expected.end());
	assertEquals( StringUtil::join(expected, '|'), _writeReadyHistory.sort().calls() );
}

TEST_CASE( "UdtEpollTest/testOverloadWait", "default" )
{
	_writeReadyHistory.clear();
	UdtEpoll epoll(&onWriteReady);

	UdtScope udt;
	UdtServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	shared_ptr<IIpSocket> clientSock = server.sock(IpAddress("127.0.0.1", 8487));
	assertFalse( !clientSock );

	// spin until failure.
	while (clientSock->try_send("hello world", 11) == 11)
		;

	assertTrue( epoll.add(*clientSock) );
	assertTrue( epoll.wait_clear() );

	assertEquals( "onWriteReady(127.0.0.1:8487)", _writeReadyHistory.calls() );
	assertEquals( 11, clientSock->send("hello world", 11) );
}

