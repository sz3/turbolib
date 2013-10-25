#pragma once

#include "UdpSocket.h"

class UdpClientSocket : public UdpSocket
{
public:
	UdpClientSocket(const IpAddress& address);
	~UdpClientSocket();
};
