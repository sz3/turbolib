/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>
class socket_address;

class udt_socket
{
public:
	udt_socket();
	udt_socket(int handle);
	udt_socket(const socket_address& endpoint);

	int handle() const;
	bool good() const;
	std::string target() const;
	socket_address endpoint() const;
	bool connect(const socket_address& endpoint);

	bool bind(const socket_address& addr);
	bool listen(int seconds);
	udt_socket accept();

	bool close();
	bool shutdown();

	int try_send(const char* buffer, unsigned size);
	int try_send(const std::string& buffer);
	int send(const char* buffer, unsigned size);
	int send(const std::string& buffer);
	int recv(char* buffer, unsigned size);

	std::string getErrorMessage() const;

protected:
	int _sock;
};
