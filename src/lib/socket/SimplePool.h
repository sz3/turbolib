#pragma once

#include "ISocketPool.h"
#include "SocketWriter.h"
#include "socket_address.h"

#include "libcuckoo/cuckoohash_map.hh"
#include <memory>

template <typename Socket>
class SimplePool : public ISocketPool<Socket>
{
protected:
	using map_type = cuckoohash_map<socket_address, int>;

public:
	std::shared_ptr<ISocketWriter> insert(const Socket& sock);
	bool insert(const Socket& sock, std::shared_ptr<ISocketWriter>& writer);

	std::shared_ptr<ISocketWriter> find(const socket_address& addr) const;

	void close(Socket& sock);
	void close_all();

protected:
	map_type _connections;
};

template <typename Socket>
std::shared_ptr<ISocketWriter> SimplePool<Socket>::insert(const Socket& sock)
{
	std::shared_ptr<ISocketWriter> conn;
	insert(sock, conn);
	return conn;
}

template <typename Socket>
bool SimplePool<Socket>::insert(const Socket& sock, std::shared_ptr<ISocketWriter>& writer)
{
	bool isnew = true;
	int handle = sock.handle();
	auto updator = [&isnew, &handle] (const int& existing) {
		isnew = false;
		handle = existing;
	};
	_connections.upsert(sock.endpoint(), updator, handle);

	writer = std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(handle));
	return isnew;
}

template <typename Socket>
std::shared_ptr<ISocketWriter> SimplePool<Socket>::find(const socket_address& addr) const
{
	int handle;
	if (!_connections.find(addr, handle))
		return NULL;
	return std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(handle));
}

template <typename Socket>
void SimplePool<Socket>::close(Socket& sock)
{
	sock.close();
	_connections.erase(sock.endpoint());
}

template <typename Socket>
void SimplePool<Socket>::close_all()
{
	for (map_type::iterator conn = _connections.begin(); !conn.is_end(); ++conn)
		Socket(conn->second).close();
	_connections.clear();
}
