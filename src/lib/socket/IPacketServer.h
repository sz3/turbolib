/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
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
