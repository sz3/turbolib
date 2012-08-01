#include "catch.hpp"
#include "UdpServer.h"
#include "UdpSocket.h"

#include <iostream>
using namespace std;

TEST_CASE( "UdpServerTest/testDefault", "default" )
{
	UdpServer server(8487);
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

