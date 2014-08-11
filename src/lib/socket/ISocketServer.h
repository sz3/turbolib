/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <memory>
#include <string>
class IpAddress;
class ISocketWriter;

class ISocketServer
{
public:
	virtual ~ISocketServer() {}

	virtual bool start() = 0;
	virtual bool stop() = 0;

	virtual std::shared_ptr<ISocketWriter> getWriter(const IpAddress& endpoint) = 0;

	virtual std::string lastError() const = 0;
};
