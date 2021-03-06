#pragma once

#include <memory>
class ISocketWriter;
class socket_address;

template <typename Socket>
class ISocketPool
{
public:
	virtual std::shared_ptr<ISocketWriter> insert(const Socket& sock) = 0;
	virtual bool insert(const Socket& sock, std::shared_ptr<ISocketWriter>& writer) = 0;

	virtual std::shared_ptr<ISocketWriter> find(const socket_address& addr) const = 0;

	virtual void close(Socket& sock) = 0;
	virtual void close_all() = 0;
};

