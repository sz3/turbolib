#pragma once

#include <string>

class IpAddress
{
public:
	IpAddress();
	IpAddress(std::string ip, unsigned short port);

	const std::string& ip() const;
	unsigned short port() const;

	std::string toString() const;
	bool fromString(const std::string& address);

protected:
	std::string _ip;
	unsigned short _port;
};
