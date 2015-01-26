/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "ISocketWriter.h"
#include "socket/socket_address.h"

template <typename Socket>
class SocketWriter : public ISocketWriter, public Socket
{
public:
	SocketWriter()
		: Socket()
	{}

	SocketWriter(const Socket& sock)
		: Socket(sock)
	{}

	int try_send(const char* buffer, unsigned size)
	{
		return Socket::try_send(buffer, size);
	}

	int send(const char* buffer, unsigned size)
	{
		return Socket::send(buffer, size);
	}

	unsigned capacity() const
	{
		return 1500;
	}

	bool flush(bool wait)
	{
		return true;
	}

	socket_address endpoint() const
	{
		return Socket::endpoint();
	}

	std::string target() const
	{
		return Socket::target();
	}

	int handle() const
	{
		return Socket::handle();
	}
};
