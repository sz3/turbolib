/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "SocketWriter.h"
#include "udp_socket.h"
using namespace std;

namespace {

}

TEST_CASE( "SocketWriterTest/testDefault", "default" )
{
	SocketWriter<udp_socket> wrapper;
}
