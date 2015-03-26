/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "tcp_socket.h"

#include "socket/socket_address.h"
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

tcp_socket::tcp_socket()
	: tcp_socket(::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))
{
}

tcp_socket::tcp_socket(int sock)
	: _sock(sock)
{
}

int tcp_socket::handle() const
{
	return _sock;
}

bool tcp_socket::good() const
{
	return _sock != -1;
}

bool tcp_socket::bind(const socket_address& addr)
{
	struct sockaddr_in local;
	memset((char*)&local, 0, sizeof(local));

	int accept_from = addr.address().empty()? INADDR_ANY : INADDR_LOOPBACK;
	local.sin_addr.s_addr = htonl(accept_from);
	local.sin_port = htons(addr.port());
	local.sin_family = AF_INET;
	return ::bind(_sock, (struct sockaddr*)&local, sizeof(local)) != -1;
}

bool tcp_socket::listen(int seconds)
{
	return ::listen(_sock, seconds) == 0;
}

tcp_socket tcp_socket::accept()
{
	struct sockaddr_in remote;
	socklen_t remote_sz = sizeof(remote);
	memset((char*)&remote, 0, remote_sz);

	int conn = ::accept(_sock, (struct sockaddr*)&remote, &remote_sz);
	return tcp_socket(conn);
}

bool tcp_socket::close()
{
	::close(_sock);
	return true;
}

bool tcp_socket::shutdown()
{
	::shutdown(_sock, SHUT_RDWR);
	return close();
}

int tcp_socket::send(const char* buffer, unsigned size)
{
	int bytes = ::send(_sock, buffer, size, 0);
	if (bytes < 0)
		perror("send");
	return bytes;
}

int tcp_socket::recv(char* buffer, unsigned size)
{
	int bytes = ::recv(_sock, buffer, size, 0);
	if (bytes < 0)
		perror("recv");
	return bytes;
}
