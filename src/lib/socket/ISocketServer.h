/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>
#include <memory>
#include <string>
class ISocketWriter;
class socket_address;

class ISocketServer
{
public:
	virtual ~ISocketServer() {}

	virtual bool start(std::function<void(ISocketWriter&, const char*, unsigned)> onPacket=NULL, std::function<bool(int)> onWriteReady=NULL) = 0;
	virtual bool stop() = 0;

	virtual std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint) = 0;
	virtual void waitForWriter(int handle) = 0;

	virtual std::string lastError() const = 0;
};
