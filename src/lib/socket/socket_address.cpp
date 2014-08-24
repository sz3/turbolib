/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "socket_address.h"

#include <sstream>

socket_address::socket_address()
	: _port(0)
{
}

socket_address::socket_address(std::string destination)
	: _address(std::move(destination))
	, _port(0)
{
}

socket_address::socket_address(std::string address, unsigned short port)
	: _address(std::move(address))
	, _port(port)
{
}

const std::string& socket_address::address() const
{
	return _address;
}

unsigned short socket_address::port() const
{
	return _port;
}

std::string socket_address::toString() const
{
	std::stringstream str;
	str << _address << ":" << _port;
	return str.str();
}

bool socket_address::fromString(const std::string& str)
{
	size_t sep = str.find_last_of(':');
	if (sep == std::string::npos || sep >= str.size()-1)
		return false;

	_address = str.substr(0, sep);
	_port = std::stoi(str.substr(sep+1));
	return true;
}
