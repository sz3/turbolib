/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <memory>
#include <string>
class ISocketWriter;
class socket_address;

class ISocketServer
{
public:
	virtual ~ISocketServer() {}

	virtual bool start() = 0;
	virtual bool stop() = 0;

	virtual std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint) = 0;
	virtual void waitForWriter(const ISocketWriter& writer) = 0;

	virtual std::string lastError() const = 0;
};
