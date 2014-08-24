/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>

class socket_address
{
public:
	socket_address();
	socket_address(std::string destination);
	socket_address(std::string address, unsigned short port);

	const std::string& address() const;
	unsigned short port() const;

	std::string toString() const;
	bool fromString(const std::string& str);

protected:
	std::string _address;
	unsigned short _port;
};

