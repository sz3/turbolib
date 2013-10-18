#pragma once

#include <string>
#include <netinet/in.h>

// maybe more a "UdpClient"...
class UdpSocket
{
public:
	UdpSocket(int sock);
	UdpSocket(const std::string& ip, short port);
	~UdpSocket();

	bool isGood() const;
	bool setTarget(const std::string& ip, short port);
	std::string getTarget() const;

	int send(const std::string& data);
	int recv(std::string& buffer);

protected:
	int _sock;
	bool _good;

	struct sockaddr_in _target;
};
