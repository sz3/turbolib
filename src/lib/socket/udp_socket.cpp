/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "udp_socket.h"

#include "IpAddress.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

udp_socket::udp_socket()
	: udp_socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
{
}

udp_socket::udp_socket(int handle)
	: _sock(handle)
{
	::memset(&_endpoint, 0, sizeof(_endpoint));
}

udp_socket::udp_socket(const IpAddress& endpoint)
	: udp_socket()
{
	setEndpoint(endpoint);
}

int udp_socket::handle() const
{
	return _sock;
}

bool udp_socket::good() const
{
	return _sock != -1;
}

IpAddress udp_socket::endpoint() const
{
	return IpAddress(inet_ntoa(_endpoint.sin_addr), ntohs(_endpoint.sin_port));
}

bool udp_socket::setEndpoint(const IpAddress& endpoint)
{
	_endpoint.sin_family = AF_INET;
	_endpoint.sin_port = htons(endpoint.port());
	return inet_aton(endpoint.ip().c_str(), &_endpoint.sin_addr) != 0;
}

bool udp_socket::bind(short port)
{
	struct sockaddr_in bind_in;
	memset((char *) &bind_in, 0, sizeof(bind_in));

	bind_in.sin_family = AF_INET;
	bind_in.sin_port = htons(port);
	bind_in.sin_addr.s_addr = htonl(INADDR_ANY);
	return ::bind(_sock, (struct sockaddr*)&bind_in, sizeof(bind_in)) != -1;
}

bool udp_socket::close()
{
	bool res = false;
	if (good())
	{
		res = ::close(_sock) != -1;
		_sock = -1;
	}
	return res;
}

bool udp_socket::shutdown()
{
	int res = ::shutdown(_sock, SHUT_RDWR);
	close();
	return res;
}

int udp_socket::try_send(const char* buffer, unsigned size)
{
	return ::sendto(_sock, buffer, size, MSG_DONTWAIT, (const sockaddr*)&_endpoint, sizeof(_endpoint));
}

int udp_socket::try_send(const std::string& buffer)
{
	return try_send(buffer.data(), buffer.size());
}

int udp_socket::send(const char* buffer, unsigned size)
{
	return ::sendto(_sock, buffer, size, 0, (const sockaddr*)&_endpoint, sizeof(_endpoint));
}

int udp_socket::send(const std::string& buffer)
{
	return send(buffer.data(), buffer.size());
}

int udp_socket::recv(char* buffer, unsigned size)
{
	socklen_t slen = sizeof(_endpoint);
	int bytes = ::recvfrom(_sock, buffer, size, 0, (sockaddr*)&_endpoint, &slen);
	if (bytes == -1)
		::perror("recvfrom failed");
	return bytes;
}

