/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "UdpSocket.h"

class UdpClientSocket : public UdpSocket
{
public:
	UdpClientSocket(const IpAddress& address);
	~UdpClientSocket();
};
