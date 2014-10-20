/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "SocketWriter.h"
#include "udp_socket.h"

TEST_CASE( "SocketWriterTest/testInstantiate", "[unit]" )
{
	SocketWriter<udp_socket> wrapper;
}
