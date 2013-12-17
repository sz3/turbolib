#include "unittest.h"

#include "UdtScope.h"
#include "UdtServer.h"
#include "UdtSocket.h"
#include "socket/IpAddress.h"
#include "util/CallHistory.h"

#include "udt/udt.h"
#include <iostream>
#include <string>
using namespace std;


namespace {
	//Event _packetEvent;
	CallHistory _serverHistory;

	void onPacket(const IIpSocket& sock, const std::string& buffer)
	{
		_serverHistory.call("onPacket", buffer);
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
