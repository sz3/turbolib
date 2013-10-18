#include "unittest.h"

#include "UdpServer.h"
#include "UdpSocket.h"
#include "common/CallHistory.h"
#include "common/CommandLine.h"

#include <iostream>
using namespace std;

namespace {
	void onPacket(UdpSocket& sock, std::string& buffer)
	{
		std::cerr << "Received packet from " << sock.getTarget() << std::endl;
		std::cerr << "Data: " << buffer << std::endl;

		sock.send(buffer);
	}
}

TEST_CASE( "UdpServerTest/testDefault", "default" )
{
	UdpServer server(8487, &onPacket);
	REQUIRE( server.start() );

	UdpSocket sock("127.0.0.1", 8487);
	REQUIRE( sock.send("hi dudes") == 8 );

	std::string buff;
	buff.resize(100);
	REQUIRE( sock.recv(buff) == 8 );

	sock.send("hi dudettes");
	sock.send("good evening, commissioner");

	server.stop();

	sock.send("laters");

	REQUIRE( buff == "hi dudes" );
}

/*
TEST_CASE( "UdpServerTest/testExternalClient", "default" )
{
	UdpServer server(8487);
	assertMsg( server.start(), server.lastError() );

	std::cerr << "hello" << std::endl;
	getchar();
	std::cerr << "goodbye" << std::endl;
	{
		string response = CommandLine::run("echo 'stfu' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: stfu\n" );
	}

	std::string buff;
	buff.resize(100);
	REQUIRE( sock.recv(buff) == 8 );
	REQUIRE( buff == "hi dudes" );
}
//*/
