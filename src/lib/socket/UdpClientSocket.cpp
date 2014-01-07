/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "UdpClientSocket.h"

#include <unistd.h>

UdpClientSocket::UdpClientSocket(const IpAddress& address)
	: UdpSocket( socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) )
{
	setTarget(address);
}

UdpClientSocket::~UdpClientSocket()
{
	close(_sock);
}
