#pragma once

#include <memory>
#include <string>
class IIpSocket;
class IpAddress;

class IPacketServer
{
public:
	virtual ~IPacketServer() {}

	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual std::string lastError() const = 0;

	virtual std::shared_ptr<IIpSocket> sock(const IpAddress& addr) = 0;
};
