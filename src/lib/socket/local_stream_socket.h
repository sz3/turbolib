/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>
class socket_address;

class local_stream_socket
{
public:
	local_stream_socket();
	local_stream_socket(int handle);

	int handle() const;
	bool good() const;

	bool connect(const socket_address& addr);
	bool bind(const socket_address& addr);
	bool listen(int seconds);
	local_stream_socket accept();

	bool close();
	bool shutdown();

	int send(const char* buffer, unsigned size);
	int recv(char* buffer, unsigned size);

protected:
	int _sock;
};
