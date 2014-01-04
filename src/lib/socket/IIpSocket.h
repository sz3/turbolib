#pragma once

#include <string>
class IpAddress;

class IIpSocket
{
public:
	virtual ~IIpSocket() {}

	virtual IpAddress getTarget() const = 0;
	virtual std::string destination() const = 0;

	virtual int send(const char* buffer, unsigned size) const = 0;
	virtual int recv(std::string& buffer) = 0;
};

