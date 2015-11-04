/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "UdtScope.h"
#include "socket/ISocketPool.h"
#include "socket/ISocketServer.h"
#include "udt_socket/udt_socket.h"
#include <functional>
#include <memory>

class UdtServer : public ISocketServer
{
public:
	UdtServer(const socket_address& addr, ISocketPool<udt_socket>* pool=NULL, unsigned numReaders=1, unsigned maxReadSize=1472);

	bool start(std::function<void(ISocketWriter&, const char*, unsigned)> onRead, std::function<bool(int)> onWriteReady);
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint);
	void waitForWriter(int id);

	std::string lastError() const;

protected:
	UdtScope _udt;
	std::unique_ptr<ISocketServer> _pimpl;
};

