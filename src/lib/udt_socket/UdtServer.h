/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "UdtScope.h"

#include "udt_socket.h"
#include "udt_socket_set.h"
#include "socket/PooledSocketServer.h"
#include "socket/SimplePool.h"

template <typename SocketPool=SimplePool<udt_socket>>
class UdtServer : public PooledSocketServer<udt_socket, udt_socket_set, SocketPool>
{
public:
	using PooledSocketServer<udt_socket, udt_socket_set, SocketPool>::PooledSocketServer;

protected:
	UdtScope _udt;
};

