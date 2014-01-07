/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "LocalStreamSocketServer.h"
#include "command_line/CommandLine.h"

#include <functional>
#include <iostream>
#include <sys/socket.h>
using namespace std;

namespace {
	void onConnect(int fd)
	{
		const int buflen = 1024;
		char buf[buflen];

		int nbytes = recv(fd, buf, buflen, 0);
		if (nbytes < 0)
			perror("recv");
		else
		{
			string message = "back at you: " + string(buf, nbytes);
			if (send(fd, message.data(), message.size(), 0) < 0)
				perror("send");
		}
	}
}

TEST_CASE( "LocalStreamSocketServerTest/testDefault", "default" )
{
	LocalStreamSocketServer server("/tmp/iamthebestserver", &onConnect, 4);
	assertMsg( server.start(), server.lastError() );

	{
		string response = CommandLine::run("echo 'stfu' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: stfu\n" );
	}

	{
		string response = CommandLine::run("echo 'again' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: again\n" );
	}
}

