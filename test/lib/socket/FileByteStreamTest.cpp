/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "FileByteStream.h"
#include "LocalStreamSocketServer.h"
#include "socket_address.h"
#include "system/popen.h"

#include <functional>
#include <iostream>
#include <sys/socket.h>
using namespace std;

namespace {
	void onConnect(local_stream_socket sock)
	{
		const int buflen = 1024;
		char buf[buflen];

		FileByteStream stream(sock.handle());
		int bytesRead = stream.read(buf, buflen);
		if (bytesRead >= 0)
		{
			string message = "back at you: " + string(buf, bytesRead);
			stream.write(message.data(), message.size());
		}
		sock.close();
	}
}

TEST_CASE( "FileByteStreamTest/testWithServer", "default" )
{
	LocalStreamSocketServer server(socket_address("/tmp/iamthebestserver"), &onConnect, 4);
	assertMsg( server.start(), server.lastError() );

	{
		string response = turbo::popen("echo 'stfu' | nc -U /tmp/iamthebestserver").read();
		assertEquals( response, "back at you: stfu\n" );
	}

	{
		string response = turbo::popen("echo 'again' | nc -U /tmp/iamthebestserver").read();
		assertEquals( response, "back at you: again\n" );
	}

}

