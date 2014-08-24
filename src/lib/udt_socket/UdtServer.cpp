#include "UdtServer.h"

#include "udt_socket.h"
#include "udt_socket_set.h"
#include "socket/PooledSocketServer.h"

UdtServer::UdtServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, unsigned numReaders, unsigned maxReadSize)
	: _pimpl(new PooledSocketServer<udt_socket, udt_socket_set>(addr, onRead, numReaders, maxReadSize))
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

std::string UdtServer::lastError() const
{
	return _pimpl->lastError();
}

