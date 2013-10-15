#include "catch.hpp"

#include "LocalStreamSocketServer.h"
#include "common/CommandLine.h"

#include <functional>
#include <iostream>
#include <sys/socket.h>
using namespace std;

namespace {
	void onConnect(int fd)
	{
		int nbytes;
		const int buflen = 1024;
		char buf[buflen];

		nbytes = recv(fd, buf, buflen, 0);
		if (nbytes < 0)
			perror("recv");
		else
		{
			string message = "back at you: " + string(buf);
			if (send(fd, message.c_str(), message.size(), 0) < 0)
				perror("send");
		}
	}
};

TEST_CASE( "LocalStreamSocketServerTest/testDefault", "default" )
{
	LocalStreamSocketServer server("/tmp/iamthebestserver", &onConnect, 4);
	bool res = server.start();
	if (!res)
		cerr << "are things bad? : " << server.lastError() << endl;
	REQUIRE( res == true );

	{
		string response = CommandLine::run("echo 'stfu' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: stfu\n" );
	}

	{
		string response = CommandLine::run("echo 'again' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: again\n" );
	}

}

