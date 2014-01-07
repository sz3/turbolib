/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "socket/IIpSocket.h"
#include <string>

class UdtSocket : public IIpSocket
{
public:
	UdtSocket(int sock);

	bool connect(const IpAddress& address);
	IpAddress getTarget() const;
	std::string destination() const;

	int send(const std::string& data) const;
	int send(const char* buffer, unsigned size) const;
	int recv(std::string& buffer);

protected:
	int _sock;
};
