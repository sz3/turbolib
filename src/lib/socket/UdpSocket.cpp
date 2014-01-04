#include "UdpSocket.h"

#include "IpAddress.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <cstring>

UdpSocket::UdpSocket(int sock)
	: _sock(sock)
	, _good(_sock != -1)
{
	::memset(&_target, 0, sizeof(_target));
}

bool UdpSocket::isGood() const
{
	return _good;
}

bool UdpSocket::setTarget(const IpAddress& address)
{
	_target.sin_family = AF_INET;
	_target.sin_port = htons(address.port());
	return inet_aton(address.ip().c_str(), &_target.sin_addr) != 0;
}

IpAddress UdpSocket::getTarget() const
{
	return IpAddress(inet_ntoa(_target.sin_addr), ntohs(_target.sin_port));
}

std::string UdpSocket::destination() const
{
	return getTarget().toString();
}

int UdpSocket::send(const std::string& data) const
{
	return send(data.data(), data.size());
}

int UdpSocket::send(const char* buffer, unsigned size) const
{
	return sendto(_sock, buffer, size, 0, (const sockaddr*)&_target, sizeof(_target));
}

int UdpSocket::recv(std::string& buffer)
{
	// TODO: recvfrom clobbers _target with new information, need to be aware of this when doing successive sends/recvs to different machines
	socklen_t slen = sizeof(_target);
	int bytes = recvfrom(_sock, &buffer[0], buffer.capacity(), 0, (sockaddr*)&_target, &slen);
	if (bytes != -1)
		buffer.resize(bytes);
	else
		::perror("recvfrom failed");
	return bytes;
}
