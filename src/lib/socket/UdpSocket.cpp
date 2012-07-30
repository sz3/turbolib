#include "UdpSocket.h"

#include <arpa/inet.h>

/*namespace {
	int sockaddr_in_size()
	{
		static int size = sizeof(struct sockaddr_in);
		return size;
	}
};*/

UdpSocket::UdpSocket(const std::string& ip, short port)
	: _sock( socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) )
	, _good( _sock != -1 )
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

bool UdpSocket::send(const std::string& data)
{
	return sendto(_sock, &data[0], data.size(), 0, (const sockaddr*)&_target, sizeof(_target)) != -1;
}
