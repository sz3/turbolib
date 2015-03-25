/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>
class socket_address;

class tcp_socket
{
public:
	tcp_socket();
	tcp_socket(int handle);

	int handle() const;
	bool good() const;

	bool bind(const socket_address& addr);
	bool listen(int seconds);
	tcp_socket accept();

	bool close();
	bool shutdown();

	int send(const char* buffer, unsigned size);
	int recv(char* buffer, unsigned size);

protected:
	int _sock;
};
