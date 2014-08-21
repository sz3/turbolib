/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>
#include <arpa/inet.h>
class IpAddress;

class udp_socket
{
public:
	udp_socket();
	udp_socket(int handle);
	udp_socket(const IpAddress& endpoint);

	int handle() const;
	bool good() const;
	std::string target() const;
	IpAddress endpoint() const;
	bool setEndpoint(const IpAddress& endpoint);

	bool bind(short port);
	bool close();
	bool shutdown();

	int try_send(const char* buffer, unsigned size);
	int try_send(const std::string& buffer);
	int send(const char* buffer, unsigned size);
	int send(const std::string& buffer);
	int recv(char* buffer, unsigned size);

protected:
	int _sock;
	struct sockaddr_in _endpoint;
};
