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

	sock.send("hi dudes");
	sock.send("hi dudettes");
	sock.send("good evening, commissioner");

	server.stop();

	sock.send("laters");


}

