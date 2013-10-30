#pragma once

#include <string>
#include <netinet/in.h>

class IpAddress;

class UdpSocket // : public IDatagramSocket
{
public:
	UdpSocket(int sock);

	bool isGood() const;
	bool setTarget(const IpAddress& address);
	IpAddress getTarget() const;

	int send(const std::string& data) const;
	int send(const char* buffer, unsigned size) const;
	int recv(std::string& buffer);

protected:
	int _sock;
	bool _good;

	struct sockaddr_in _target;
};
