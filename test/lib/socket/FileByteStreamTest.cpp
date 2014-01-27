/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "FileByteStream.h"
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

		FileByteStream stream(fd);
		int bytesRead = stream.read(buf, buflen);
		if (bytesRead >= 0)
		{
			string message = "back at you: " + string(buf, bytesRead);
			stream.write(message.data(), message.size());
		}
	}
}

TEST_CASE( "FileByteStreamTest/testWithServer", "default" )
{
	LocalStreamSocketServer server("/tmp/iamthebestserver", &onConnect, 4);
	assertMsg( server.start(), server.lastError() );

	{
		string response = CommandLine::run("echo 'stfu' | nc -U /tmp/iamthebestserver");
		assertEquals( response, "back at you: stfu\n" );
	}

	{
		string response = CommandLine::run("echo 'again' | nc -U /tmp/iamthebestserver");
		assertEquals( response, "back at you: again\n" );
	}

}

