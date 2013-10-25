#include "IpAddress.h"

#include <sstream>

IpAddress::IpAddress()
	: _port(0)
{
}

IpAddress::IpAddress(const std::string& ip, short port)
	: _ip(ip)
	, _port(port)
{
}

const std::string& IpAddress::ip() const
{
	return _ip;
}

short IpAddress::port() const
{
	return _port;
}

std::string IpAddress::toString() const
{
	std::stringstream str;
	str << _ip << ":" << _port;
	return str.str();
}

bool IpAddress::fromString(const std::string& address)
{
	size_t sep = address.find_last_of(':');
	if (sep == std::string::npos || sep >= address.size()-1)
		return false;

	_ip = address.substr(0, sep);
	_port = std::stoi(address.substr(sep+1));
	return true;
}
