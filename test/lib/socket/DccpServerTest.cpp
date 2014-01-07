/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "DccpServer.h"
#include "DccpSocket.h"

#include "IpAddress.h"
#include "util/CallHistory.h"
#include <arpa/inet.h>

#include <iostream>
using namespace std;

namespace {
	CallHistory _serverHistory;

	void onPacket(const IIpSocket& sock, const std::string& buffer)
	{
		_serverHistory.call("onPacket", buffer);
		sock.send(buffer.data(), buffer.size());
	}
}

TEST_CASE( "DccpServerTest/testDefault", "default" )
{
	DccpServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	int clientSock = ::socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	DccpSocket sock(clientSock);
	assertTrue( sock.connect( IpAddress("127.0.0.1", 8487) ) );
	assertEquals( 8, sock.send("hi world") );

	string response;
	response.resize(100);
	assertEquals( 8, sock.recv(response) );
	assertEquals( "hi world", response );
	assertEquals( "onPacket(hi world)", _serverHistory.calls() );
}

TEST_CASE( "DccpServerTest/testServerCrosstalk", "default" )
{
	DccpServer server(8487, &onPacket);
	assertMsg( server.start(), server.lastError() );

	DccpServer other(8488, &onPacket);
	assertMsg( other.start(), other.lastError() );

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
