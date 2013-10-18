#pragma once

#include "UdpSocket.h"

class UdpClientSocket : public UdpSocket
{
public:
	UdpClientSocket(const std::string& ip, short port);
	~UdpClientSocket();
};
