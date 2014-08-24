/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "StreamSocketAcceptorServer.h"
#include "local_stream_socket.h"

typedef StreamSocketAcceptorServer<local_stream_socket> LocalStreamSocketServer;

