/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "StatelessSocketServer.h"
#include "udp_socket.h"

typedef StatelessSocketServer<udp_socket> UdpServer;

