#pragma once

#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

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
