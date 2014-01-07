/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IIpSocket.h"
#include <string>
#include <netinet/in.h>

class IpAddress;

class UdpSocket : public IIpSocket
{
public:
	UdpSocket(int sock);

	bool isGood() const;
	bool setTarget(const IpAddress& address);
	IpAddress getTarget() const;
	std::string destination() const;

	int send(const std::string& data) const;
	int send(const char* buffer, unsigned size) const;
	int recv(std::string& buffer);

protected:
	int _sock;
	bool _good;

	struct sockaddr_in _target;
};
