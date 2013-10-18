#include "UdpSocket.h"

#include <arpa/inet.h>
#include <iostream>
#include <sstream>

/*namespace {
	int sockaddr_in_size()
	{
		static int size = sizeof(struct sockaddr_in);
		return size;
	}
};*/

UdpSocket::UdpSocket(int sock)
	: _sock(sock)
	, _good(_sock != -1)
{
}

UdpSocket::UdpSocket(const std::string& ip, short port)
	: _sock( socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) )
	, _good(_sock != -1)
{
	setTarget(ip, port);
}

UdpSocket::~UdpSocket()
{
	close(_sock);
}

bool UdpSocket::isGood() const
{
	return _good;
}

bool UdpSocket::setTarget(const std::string& ip, short port)
{
	_target.sin_family = AF_INET;
	_target.sin_port = htons(port);
	return inet_aton(ip.c_str(), &_target.sin_addr) != 0;
}

std::string UdpSocket::getTarget() const
{
	std::stringstream ss;
	ss << inet_ntoa(_target.sin_addr) << ":" << ntohs(_target.sin_port);
	return ss.str();
}

int UdpSocket::send(const std::string& data)
{
	return sendto(_sock, &data[0], data.size(), 0, (const sockaddr*)&_target, sizeof(_target));
}

int UdpSocket::recv(std::string& buffer)
{
	// TODO: recvfrom clobbers _target with new information, need to be aware of this when doing successive sends/recvs to different machines
	socklen_t slen = sizeof(_target);
	int bytes = recvfrom(_sock, &buffer[0], buffer.capacity(), 0, (sockaddr*)&_target, &slen);
	if (bytes != -1)
		buffer.resize(bytes);
	return bytes;
}
