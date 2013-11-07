#pragma once

#include "IpAddress.h"
#include <string>

class IIpSocket
{
public:
	virtual ~IIpSocket() {}

	virtual bool setTarget(const IpAddress& address) = 0;
	virtual IpAddress getTarget() const = 0;

	virtual int send(const char* buffer, unsigned size) const = 0;
	virtual int recv(std::string& buffer) = 0;
};
