/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "DccpSocket.h"

#include "udt/udt.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <cstring>

DccpSocket::DccpSocket(int sock)
	: _sock(sock)
{
}

DccpSocket::DccpSocket(int sock, const IpAddress& target)
	: _sock(sock)
	, _target(target)
{
}

bool DccpSocket::connect(const IpAddress& address)
{
	_target = address;

	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(address.port());
	inet_aton(address.ip().c_str(), &target.sin_addr);

	int res = ::connect(_sock, (const sockaddr*)&target, sizeof(target));
	if (res != 0)
		std::cout << "DccpSocket::connect crashed and burned. " << res << std::endl;
	return res == 0;
}

IpAddress DccpSocket::getTarget() const
{
	return _target;
}

int DccpSocket::send(const std::string& data) const
{
	return send(data.data(), data.size());
}

int DccpSocket::send(const char* buffer, unsigned size) const
{
	return ::send(_sock, buffer, size, 0);
}

int DccpSocket::recv(std::string& buffer)
{
	int bytes = ::recv(_sock, &buffer[0], buffer.capacity(), 0);
	if (bytes >= 0)
		buffer.resize(bytes);
	else
		std::cout << "DccpSocket::recv crashed and burned. " << bytes << std::endl;
	return bytes;
}
