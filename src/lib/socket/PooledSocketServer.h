/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

// for tcp, udt, dccp, anything that uses an accept() -> epoll() pattern

#include "ISocketServer.h"

#include "IpAddress.h"
#include "SocketWriter.h"
#include "event/Event.h"
#include "serialize/StringUtil.h"

#include "tbb/concurrent_unordered_map.h"
#include <deque>
#include <functional>
#include <iostream>
#include <thread>

template <typename Socket, typename SocketSet>
class PooledSocketServer : public ISocketServer
{
public:
	PooledSocketServer(short port, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, unsigned numReaders=1, unsigned maxReadSize=1450);
	~PooledSocketServer();

	bool start();
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const IpAddress& endpoint);

	bool running() const;
	std::string lastError() const;

protected:
	void accept();
	void run();
	bool fatalError(const std::string& error);

protected:
	Socket _sock;
	SocketSet _socketSet;
	tbb::concurrent_unordered_map<std::string,int> _connections;
	bool _running;
	short _port;

	std::function<void(ISocketWriter&, const char*, unsigned)> _onRead;
	unsigned _maxReadSize;

	Event _started;
	unsigned _numReaders;
	std::deque<std::thread> _runners;
	std::thread _acceptor;
	std::string _lastError;
};

template <typename Socket, typename SocketSet>
PooledSocketServer<Socket,SocketSet>::PooledSocketServer(short port, std::function<void(ISocketWriter&, const char*, unsigned)> onRead, unsigned numReaders, unsigned maxReadSize)
	: _running(false)
	, _port(port)
	, _socketSet(SocketSet::READS)
	, _onRead(onRead)
	, _numReaders(numReaders)
	, _maxReadSize(maxReadSize)
{

}

template <typename Socket, typename SocketSet>
PooledSocketServer<Socket,SocketSet>::~PooledSocketServer()
{
	stop();
}

template <typename Socket, typename SocketSet>
bool PooledSocketServer<Socket,SocketSet>::start()
{
	if (_running)
		return true;

	_running = true;
	if (!_sock.good())
		return fatalError("couldn't get good socket");

	if (!_socketSet.good())
	{
		_sock.close();
		return fatalError("couldn't get epoll to work for reads: " + _socketSet.lastError());
	}

	if (!_sock.bind(_port))
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

template <typename Socket, typename SocketSet>
bool PooledSocketServer<Socket,SocketSet>::stop()
{
	if (!_running)
		return false;

	_running = false;
	_socketSet.release();
	_sock.close();
	for (auto conn = _connections.begin(); conn != _connections.end(); ++conn)
		Socket(conn->second).close();

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

template <typename Socket, typename SocketSet>
void PooledSocketServer<Socket,SocketSet>::accept()
{
	_started.shutdown();
	while (_running)
	{
		Socket conn = _sock.accept();
		if (!conn.good())
			continue;

		if (!_socketSet.add(conn.handle()))
			std::cout << "couldn't add socket to socket_set." << std::endl;
	}
}

template <typename Socket, typename SocketSet>
void PooledSocketServer<Socket,SocketSet>::run()
{
	std::string buffer;
	buffer.resize(_maxReadSize);

	while (_running)
	{
		std::set<int> reads = _socketSet.wait();
		for (std::set<int>::const_iterator it = reads.begin(); it != reads.end(); ++it)
		{
			Socket sock(*it);
			int bytesRead = sock.recv(&buffer[0], buffer.size());
			if (bytesRead <= 0)
			{
				_socketSet.remove(*it);
				sock.close();
				continue;
			}

			SocketWriter<Socket> writer(sock);
			_onRead(writer, &buffer[0], bytesRead);
			_connections[sock.endpoint().toString()] = *it;
		}
	}
}

template <typename Socket, typename SocketSet>
bool PooledSocketServer<Socket,SocketSet>::running() const
{
	return _running;
}

template <typename Socket, typename SocketSet>
std::shared_ptr<ISocketWriter> PooledSocketServer<Socket,SocketSet>::getWriter(const IpAddress& endpoint)
{
	// if we already got one, return it
	std::pair< tbb::concurrent_unordered_map<std::string, int>::iterator, bool> pear = _connections.insert( {endpoint.toString(), -1} );
	if (pear.first->second >= 0)
		return std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(pear.first->second));

	Socket sock(endpoint);
	if (!sock.good())
	{
		std::cout << "socket getWriter failed miserably: " << sock.getErrorMessage() << std::endl;
		return NULL;
	}
	pear.first->second = sock.handle();

	if (!_socketSet.add(sock.handle()))
		std::cout << "socketSet add error for client sock: " << sock.getErrorMessage() << std::endl;
	return std::shared_ptr<ISocketWriter>(new SocketWriter<Socket>(sock));
}

template <typename Socket, typename SocketSet>
std::string PooledSocketServer<Socket,SocketSet>::lastError() const
{
	return _lastError;
}

template <typename Socket, typename SocketSet>
bool PooledSocketServer<Socket,SocketSet>::fatalError(const std::string& error)
{
	_lastError = error;
	return _running = false;
}
