/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "UdtScope.h"
#include "socket/ISocketServer.h"
#include <functional>
#include <memory>

class UdtServer : public ISocketServer
{
public:
	UdtServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, unsigned numReaders=1, unsigned maxReadSize=1450);

	bool start();
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint);

	std::string lastError() const;

protected:
	UdtScope _udt;
	std::unique_ptr<ISocketServer> _pimpl;
};

