/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

// for tcp, udt, dccp, anything that uses an accept() -> epoll() pattern

#include "ISocketServer.h"

#include "SimplePool.h"
#include "SocketWriter.h"
#include "event/Event.h"
#include "serialize/StringUtil.h"

#include "tbb/concurrent_unordered_map.h"
#include <deque>
#include <functional>
#include <iostream>
#include <thread>

template <typename Socket, typename SocketSet, typename SocketPool=SimplePool<Socket>>
class PooledSocketServer : public ISocketServer
{
public:
	PooledSocketServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, SocketPool* pool=NULL, unsigned numReaders=1, unsigned maxReadSize=1450);
	~PooledSocketServer();

	bool start();
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint);

	bool running() const;
	std::string lastError() const;

protected:
	void accept();
	void run();
	bool fatalError(const std::string& error);

protected:
	std::unique_ptr<SocketPool> _defaultPool;

protected:
	SocketPool& _pool;
	Socket _sock;
	SocketSet _readSet;
	bool _running;
	socket_address _addr;

	std::function<void(ISocketWriter&, const char*, unsigned)> _onRead;
	unsigned _maxReadSize;

	Event _started;
	unsigned _numReaders;
	std::deque<std::thread> _runners;
	std::thread _acceptor;
	std::string _lastError;
};

template <typename Socket, typename SocketSet, typename SocketPool>
PooledSocketServer<Socket,SocketSet,SocketPool>::PooledSocketServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, SocketPool* pool, unsigned numReaders, unsigned maxReadSize)
	: _running(false)
	, _addr(addr)
	, _readSet(SocketSet::READS)
	, _onRead(onRead)
	, _numReaders(numReaders)
	, _maxReadSize(maxReadSize)
	, _defaultPool(pool == NULL? new SocketPool() : NULL)
	, _pool(pool == NULL? *_defaultPool : *pool)
{

}

template <typename Socket, typename SocketSet, typename SocketPool>
PooledSocketServer<Socket,SocketSet,SocketPool>::~PooledSocketServer()
{
	stop();
}

template <typename Socket, typename SocketSet, typename SocketPool>
bool PooledSocketServer<Socket,SocketSet,SocketPool>::start()
{
	if (_running)
		return true;

	_running = true;
	if (!_sock.good())
		return fatalError("couldn't get good socket");

	if (!_readSet.good())
	{
		_sock.close();
		return fatalError("couldn't get epoll to work for reads: " + _readSet.lastError());
	}

	if (!_sock.bind(_addr))
	{
		_sock.close();
		return fatalError("couldn't bind to port: " + _sock.getErrorMessage());
	}

	if (!_sock.listen(20))
	{
		_sock.close();
		return fatalError("couldn't get socket to listen: " + _sock.getErrorMessage());
	}

	_acceptor = std::thread( std::bind(&PooledSocketServer<Socket,SocketSet>::accept, this) );
	for (unsigned i = 0; i < _numReaders; ++i)
		_runners.push_back( std::thread(std::bind(&PooledSocketServer<Socket,SocketSet>::run, this)) );
	_started.wait();
	return _running;
}

template <typename Socket, typename SocketSet, typename SocketPool>
bool PooledSocketServer<Socket,SocketSet,SocketPool>::stop()
{
	if (!_running)
		return false;

	_running = false;
	_readSet.release();
	_sock.close();
	_pool.close_all();

	if (_acceptor.joinable())
		_acceptor.join();
	for (auto it = _runners.begin(); it != _runners.end(); ++it)
	{
		 if (it->joinable())
			it->join();
	}
	_runners.clear();
	return true;
}

template <typename Socket, typename SocketSet, typename SocketPool>
void PooledSocketServer<Socket,SocketSet,SocketPool>::accept()
{
	_started.shutdown();
	while (_running)
	{
		Socket conn = _sock.accept();
		if (!conn.good())
			continue;

		_pool.add(conn);
		if (!_readSet.add(conn.handle()))
			std::cout << "couldn't add socket to socket_set." << std::endl;
	}
}

template <typename Socket, typename SocketSet, typename SocketPool>
void PooledSocketServer<Socket,SocketSet,SocketPool>::run()
{
	std::string buffer;
	buffer.resize(_maxReadSize);

	while (_running)
	{
		std::set<int> reads = _readSet.wait();
		for (std::set<int>::const_iterator it = reads.begin(); it != reads.end(); ++it)
		{
			Socket sock(*it);
			int bytesRead = sock.recv(&buffer[0], buffer.size());
			if (bytesRead <= 0)
			{
				_readSet.remove(*it);
				_pool.close(sock);
				continue;
			}

			std::shared_ptr<ISocketWriter> writer = _pool.find_or_add(sock);
			_onRead(*writer, &buffer[0], bytesRead);
		}
	}
}

template <typename Socket, typename SocketSet, typename SocketPool>
bool PooledSocketServer<Socket,SocketSet,SocketPool>::running() const
{
	return _running;
}

template <typename Socket, typename SocketSet, typename SocketPool>
std::shared_ptr<ISocketWriter> PooledSocketServer<Socket,SocketSet,SocketPool>::getWriter(const socket_address& endpoint)
{
	// we have an essential race: during the time we're trying to connect, we might get a connection to that peer.
	// in that case, use the server-derived connection.

	std::shared_ptr<ISocketWriter> writer = _pool.find(endpoint);
	if (!!writer)
		return writer;

	// else, create new socket
	Socket sock(endpoint);
	if (!sock.good())
	{
		std::cout << "socket getWriter failed miserably: " << sock.getErrorMessage() << std::endl;
		return NULL;
	}

	// try to add it
	if (!_pool.add(sock, writer))
		sock.close(); // welp, race condition
	else if (!_readSet.add(sock.handle()))
	{
		std::cout << "socketSet add error for client sock: " << sock.getErrorMessage() << std::endl;
		sock.close();
		return NULL;
	}
	return writer;
}

template <typename Socket, typename SocketSet, typename SocketPool>
std::string PooledSocketServer<Socket,SocketSet,SocketPool>::lastError() const
{
	return _lastError;
}

template <typename Socket, typename SocketSet, typename SocketPool>
bool PooledSocketServer<Socket,SocketSet,SocketPool>::fatalError(const std::string& error)
{
	_lastError = error;
	return _running = false;
}
