#pragma once

#include "socket/IIpSocket.h"
#include <string>

class UdtSocket : public IIpSocket
{
public:
	UdtSocket(int sock);

	bool connect(const IpAddress& address);
	IpAddress getTarget() const;

	int send(const std::string& data) const;
	int send(const char* buffer, unsigned size) const;
	int recv(std::string& buffer);

protected:
	int _sock;
};
