/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "UdtScope.h"
#include "UdtServer.h"
#include "UdtSocket.h"
#include "serialize/StringUtil.h"
#include "socket/IpAddress.h"
#include "time/WaitFor.h"
#include "util/CallHistory.h"

#include "udt/udt.h"
#include <iostream>
#include <string>
using namespace std;


namespace {
	//Event _packetEvent;
	CallHistory _serverHistory;
	IpAddress _serverLastAddr;

	void onPacket(const IIpSocket& sock, const std::string& buffer)
	{
		_serverHistory.call("onPacket", buffer);
		_serverLastAddr = sock.getTarget();
		//_packetEvent.signal();
		sock.send(buffer.data(), buffer.size());
	}
}

TEST_CASE( "UdtServerTest/testDefault", "default" )
{
	UdtScope udt;

	UdtServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	UDTSOCKET handle = UDT::socket(AF_INET, SOCK_DGRAM, 0);
	assertMsg( handle != UDT::INVALID_SOCK, UDT::getlasterror().getErrorMessage() );

	UdtSocket sock(handle);
	assertTrue( sock.connect(IpAddress("127.0.0.1", 8487)) );
	assertEquals( 11, sock.send("hello world") );

	string buff;
	buff.resize(100);
	assertEquals( 11, sock.recv(buff) );
	assertEquals( "hello world", buff );

	//_packetEvent.wait(5000);
	assertEquals( "onPacket(hello world)", _serverHistory.calls() );
}

TEST_CASE( "UdtServerTest/testServerCrosstalk", "default" )
{
	UdtScope udt;
	_serverHistory.clear();

	UdtServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	UdtServer other(8488, &onPacket);
	assertMsg( other.start(), other.lastError() );

	{
		std::shared_ptr<IIpSocket> sock( other.sock(IpAddress("127.0.0.1", 8487)) );
		assertNotNull( sock.get() );

		string request = "hi world";
		assertEquals( 8, sock->send(request.data(), request.size()) );

		string response;
		response.resize(100);
		assertEquals( 8, sock->recv(response) );
		assertEquals( "hi world", response );
		assertEquals( "onPacket(hi world)", _serverHistory.calls() );
	}

	// and then the other way

	{
		std::shared_ptr<IIpSocket> sock( server.sock(_serverLastAddr) );
		assertNotNull( sock.get() );

		string request = "bye world";
		assertEquals( 9, sock->send(request.data(), request.size()) );

		string response;
		response.resize(100);
		assertEquals( 9, sock->recv(response) );
		assertEquals( "bye world", response );
		assertEquals( "onPacket(hi world)|onPacket(bye world)", _serverHistory.calls() );

		assertEquals( "127.0.0.1:8487", _serverLastAddr.toString() );
	}
}

TEST_CASE( "UdtServerTest/testSpam", "default" )
{
	UdtScope udt;

	unsigned bytesRecv = 0;
	auto fun = [&bytesRecv] (const IIpSocket& sock, const std::string& buffer) { bytesRecv += buffer.size(); };

	UdtServer server(8487, fun);
	assertMsg( server.start(), server.lastError() );

	UDTSOCKET handle = UDT::socket(AF_INET, SOCK_DGRAM, 0);
	assertMsg( handle != UDT::INVALID_SOCK, UDT::getlasterror().getErrorMessage() );

	UdtSocket sock(handle);
	assertTrue( sock.connect(IpAddress("127.0.0.1", 8487)) );

	string packet = "01234567890123456789012345678901234567890123456789abcdef";
	for (int i = 0; i < 100000; ++i)
		assertEquals( 56, sock.send(packet.data(), packet.size()) );

	unsigned expected = 56*100000;
	waitFor(2, StringUtil::str(expected) + " != " + StringUtil::str(bytesRecv), [&]()
	{
		return expected == bytesRecv;
	});
}
