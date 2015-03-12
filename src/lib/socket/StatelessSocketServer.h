/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

// e.g. for udp
#include "ISocketServer.h"

#include "SimplePool.h"
#include "SocketWriter.h"
#include "serialize/str.h"

#include <deque>
#include <functional>
#include <thread>

template <typename Socket>
class StatelessSocketServer : public ISocketServer
{
public:
	StatelessSocketServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, ISocketPool<Socket>* pool=NULL, unsigned numReaders=1, unsigned maxReadSize=1472);
	~StatelessSocketServer();

	bool start();
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint);
	void waitForWriter(int id) {}

	bool running() const;
	std::string lastError() const;

protected:
	void run();
	bool fatalError(const std::string& error);

protected:
	std::unique_ptr<ISocketPool<Socket>> _poolPtr;

protected:
	ISocketPool<Socket>& _pool;
	Socket _sock;
	bool _running;
	socket_address _addr;

	std::function<void(ISocketWriter&, const char*, unsigned)> _onRead;
	unsigned _maxReadSize;

	unsigned _numReaders;
	std::deque<std::thread> _readers;
	std::string _lastError;
};

template <typename Socket>
StatelessSocketServer<Socket>::StatelessSocketServer(const socket_address& addr, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, ISocketPool<Socket>* pool, unsigned numReaders, unsigned maxReadSize)
	: _running(false)
	, _addr(addr)
	, _onRead(onRead)
	, _numReaders(numReaders)
	, _maxReadSize(maxReadSize)
	, _poolPtr(pool == NULL? new SimplePool<Socket>() : pool)
	, _pool(*_poolPtr)
{

}

template <typename Socket>
StatelessSocketServer<Socket>::~StatelessSocketServer()
{
	stop();
}

template <typename Socket>
bool StatelessSocketServer<Socket>::start()
{
	if (_running)
		return true;

	_running = true;
	if (!_sock.good())
		return fatalError("couldn't get good socket");

	if (!_sock.bind(_addr))
	{
		_sock.close();
		return fatalError("couldn't bind socket to port " + turbo::str::str(_addr.port()));
	}

	for (unsigned i = 0; i < _numReaders; ++i)
		_readers.push_back( std::thread(std::bind(&StatelessSocketServer<Socket>::run, this)) );
	return _running;
}

template <typename Socket>
bool StatelessSocketServer<Socket>::stop()
{
	if (!_running)
		return false;

	_running = false;
	_sock.shutdown();

	for (auto it = _readers.begin(); it != _readers.end(); ++it)
	{
		 if (it->joinable())
			it->join();
	}
	_readers.clear();
	return true;
}

template <typename Socket>
void StatelessSocketServer<Socket>::run()
{
	SocketWriter<Socket> sock(_sock);
	std::string buffer;
	buffer.resize(_maxReadSize);

	while (_running)
	{
		int bytesRead = sock.recv(&buffer[0], buffer.size());
		if (bytesRead <= 0)
			continue;

		_onRead(sock, &buffer[0], bytesRead);
	}
}

template <typename Socket>
bool StatelessSocketServer<Socket>::running() const
{
	return _running;
}

template <typename Socket>
std::shared_ptr<ISocketWriter> StatelessSocketServer<Socket>::getWriter(const socket_address& endpoint)
{
	// we have an essential race: we might have multiple callers to getWriter
	std::shared_ptr<ISocketWriter> writer = _pool.find(endpoint);
	if (!!writer)
		return writer;

	// else, create new socket
	Socket sock(_sock);
	sock.setEndpoint(endpoint);

	// try to add it
	_pool.add(sock, writer);
	return writer;
}

template <typename Socket>
std::string StatelessSocketServer<Socket>::lastError() const
{
	return _lastError;
}

template <typename Socket>
bool StatelessSocketServer<Socket>::fatalError(const std::string& error)
{
	_lastError = error;
	return _running = false;
}
