#include "UdpClientSocket.h"

UdpClientSocket::UdpClientSocket(const std::string& ip, short port)
	: UdpSocket( socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) )
{
	setTarget(ip, port);
}

UdpClientSocket::~UdpClientSocket()
{
	close(_sock);
}
