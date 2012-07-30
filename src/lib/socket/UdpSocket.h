#pragma once

#include <string>
#include <netinet/in.h>

// maybe more a "UdpClient"...
class UdpSocket
{
public:
	UdpSocket(const std::string& ip, short port);
	~UdpSocket();

	bool isGood() const;
	bool setTarget(const std::string& ip, short port);

	bool send(const std::string& data);

protected:
	int _sock;
	bool _good;

	struct sockaddr_in _target;
};
