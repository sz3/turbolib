#include "unittest.h"

#include "ConnectionByteStream.h"
#include "LocalStreamSocketServer.h"
#include "common/CommandLine.h"

#include <functional>
#include <iostream>
#include <sys/socket.h>
using namespace std;

namespace {
	void onConnect(int fd)
	{
		const int buflen = 1024;
		char buf[buflen];

		ConnectionByteStream stream(fd);
		if (stream.read(buf, buflen) >= 0)
		{
			string message = "back at you: " + string(buf);
			stream.write(message.c_str(), message.size());
		}
	}
}

TEST_CASE( "ConnectionByteStreamTest/testWithServer", "default" )
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

