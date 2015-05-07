/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "UdtServer.h"

#include "udt_socket.h"
#include "udt_socket_set.h"
#include "socket/PooledSocketServer.h"

UdtServer::UdtServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, std::function<bool(int)> onWriteReady, ISocketPool<udt_socket>* pool, unsigned numReaders, unsigned maxReadSize)
	: _pimpl(new PooledSocketServer<udt_socket, udt_socket_set>(addr, onRead, onWriteReady, pool, numReaders, maxReadSize))
{
}

bool UdtServer::start()
{
	return _pimpl->start();
}

bool UdtServer::stop()
{
	return _pimpl->stop();
}

std::shared_ptr<ISocketWriter> UdtServer::getWriter(const socket_address& endpoint)
{
	return _pimpl->getWriter(endpoint);
}

void UdtServer::waitForWriter(int id)
{
	_pimpl->waitForWriter(id);
}

std::string UdtServer::lastError() const
{
	return _pimpl->lastError();
}

