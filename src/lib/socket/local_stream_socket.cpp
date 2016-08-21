/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "local_stream_socket.h"

#include "socket/socket_address.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// see http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html#unixsockserv

local_stream_socket::local_stream_socket()
	: local_stream_socket(::socket(PF_UNIX, SOCK_STREAM, 0))
{
}

local_stream_socket::local_stream_socket(int sock)
	: _sock(sock)
{
}

int local_stream_socket::handle() const
{
	return _sock;
}

bool local_stream_socket::good() const
{
	return _sock != -1;
}

bool local_stream_socket::connect(const socket_address& addr)
{
	std::string filename = addr.address();

	struct sockaddr_un remote;
	memset((char*) &remote, 0, sizeof(remote));

	remote.sun_family = AF_UNIX;
	strncpy(remote.sun_path, filename.c_str(), filename.size()+1);
	return ::connect(_sock, (struct sockaddr*)&remote, sizeof(remote)) == 0;
}

bool local_stream_socket::bind(const socket_address& addr)
{
	std::string filename = addr.address();
	::unlink(filename.c_str());

	struct sockaddr_un local;
	memset((char*) &local, 0, sizeof(local));

	local.sun_family = AF_UNIX;
	strncpy(local.sun_path, filename.c_str(), filename.size()+1);
	return ::bind(_sock, (struct sockaddr*)&local, sizeof(local)) != -1;
}

bool local_stream_socket::listen(int seconds)
{
	return ::listen(_sock, seconds) == 0;
}

local_stream_socket local_stream_socket::accept()
{
	struct sockaddr_un remote;
	socklen_t remote_sz = sizeof(remote);
	memset((char*)&remote, 0, remote_sz);

	int conn = ::accept(_sock, (struct sockaddr*)&remote, &remote_sz);
	return local_stream_socket(conn);
}

bool local_stream_socket::close()
{
	::close(_sock);
	return true;
}

bool local_stream_socket::shutdown()
{
	::shutdown(_sock, SHUT_RDWR);
	return close();
}

int local_stream_socket::send(const char* buffer, unsigned size)
{
	int bytes = ::send(_sock, buffer, size, MSG_NOSIGNAL);
	if (bytes < 0)
		perror("send");
	return bytes;
}

int local_stream_socket::recv(char* buffer, unsigned size)
{
	int bytes = ::recv(_sock, buffer, size, 0);
	if (bytes < 0)
		perror("recv");
	return bytes;
}
