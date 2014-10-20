#pragma once

#include "SocketWriter.h"
#include "socket_address.h"
#include "tbb/concurrent_unordered_map.h"
#include <memory>

template <typename Socket>
class SimplePool
{
protected:
	using map_type = tbb::concurrent_unordered_map<std::string,int>; // concurrent_hash_map?

public:
	bool add(const Socket& sock);
	bool add(const Socket& sock, std::shared_ptr<ISocketWriter>& writer);

	std::shared_ptr<ISocketWriter> find(const socket_address& addr) const;
	std::shared_ptr<ISocketWriter> find_or_add(const Socket& sock);

	void close(Socket& sock);
	void close_all();

protected:
	map_type::const_iterator _connections_find(const socket_address& addr) const;
	map_type::iterator _connections_find(const socket_address& addr);

protected:
	map_type _connections;
};

template <typename Socket>
bool SimplePool<Socket>::add(const Socket& sock)
{
	std::shared_ptr<ISocketWriter> writer;
	return add(sock, writer);
}

template <typename Socket>
bool SimplePool<Socket>::add(const Socket& sock, std::shared_ptr<ISocketWriter>& writer)
{
	bool isnew = false;
	std::pair< map_type::iterator, bool> pear = _connections.insert( {sock.endpoint().toString(), -1} );
	if (pear.first->second < 0)
	{
		pear.first->second = sock.handle();
		isnew = true;
	}
	writer.reset(new SocketWriter<Socket>(pear.first->second));
	return isnew;
}

template <typename Socket>
SimplePool<Socket>::map_type::const_iterator SimplePool<Socket>::_connections_find(const socket_address& addr) const
{
	return _connections.find(addr.toString());
}

template <typename Socket>
SimplePool<Socket>::map_type::iterator SimplePool<Socket>::_connections_find(const socket_address& addr)
{
	return _connections.find(addr.toString());
}

template <typename Socket>
std::shared_ptr<ISocketWriter> SimplePool<Socket>::find(const socket_address& addr) const
{
	map_type::const_iterator it = _connections_find(addr);
	if (it == _connections.end())
		return NULL;
	return std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(it->second));
}

template <typename Socket>
std::shared_ptr<ISocketWriter> SimplePool<Socket>::find_or_add(const Socket& sock)
{
	map_type::iterator it = _connections_find(sock.endpoint());
	if (it == _connections.end())
	{
		std::shared_ptr<ISocketWriter> res;
		add(sock, res); // TODO: this is... bad?
		return res;
	}
	return std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(it->second));
}

template <typename Socket>
void SimplePool<Socket>::close(Socket& sock)
{
	sock.close();
	map_type::iterator it = _connections_find(sock.endpoint());
	if (it != _connections.end())
		_connections.unsafe_erase(it);
}

template <typename Socket>
void SimplePool<Socket>::close_all()
{
	for (map_type::iterator conn = _connections.begin(); conn != _connections.end(); ++conn)
		Socket(conn->second).close();
	_connections.clear();
}
