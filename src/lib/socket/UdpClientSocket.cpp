#include "UdpClientSocket.h"

UdpClientSocket::UdpClientSocket(const IpAddress& address)
	: UdpSocket( socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) )
{
	setTarget(address);
}

UdpClientSocket::~UdpClientSocket()
{
	close(_sock);
}
