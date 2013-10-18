#include "unittest.h"

#include "UdpServer.h"
#include "UdpClientSocket.h"
#include "command_line/CommandLine.h"
#include "util/CallHistory.h"

#include <iostream>
using namespace std;

namespace {
	CallHistory _serverHistory;

	void onPacket(UdpSocket& sock, std::string& buffer)
	{
		_serverHistory.call("onPacket", buffer);
		sock.send(buffer);
	}
}

TEST_CASE( "UdpServerTest/testDefault", "default" )
{
	UdpServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	UdpClientSocket sock("127.0.0.1", 8487);

	assertEquals( 8, sock.send("hi dudes") );
	std::string buff;
	buff.resize(100);
	assertEquals( 8, sock.recv(buff) );
	assertEquals( "hi dudes", buff );

	assertEquals( 11, sock.send("hi dudettes") );
	buff.resize(100);
	assertEquals( 11, sock.recv(buff) );
	assertEquals( "hi dudettes", buff );

	assertEquals( 26, sock.send("good evening, commissioner") );
	buff.resize(100);
	assertEquals( 26, sock.recv(buff) );
	assertEquals( "good evening, commissioner", buff );

	server.stop();

	// TODO: what?
	assertEquals( 6, sock.send("laters") );

	assertEquals( "onPacket(hi dudes)|onPacket(hi dudettes)|onPacket(good evening, commissioner)", _serverHistory.calls() );
}

TEST_CASE( "UdpServerTest/testExternalClient", "default" )
{
	_serverHistory.clear();
	UdpServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	// -w 0 option makes nc go away immediately. So don't listen for a response, lulz!
	CommandLine::run("echo 'stfu' | nc -u 127.0.0.1 8487 -w 0");
	assertEquals( "onPacket(stfu\n)", _serverHistory.calls() );
}
