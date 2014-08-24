/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "ISocketServer.h"
#include "socket_address.h"
#include <deque>
#include <functional>
#include <thread>

template <typename Socket>
class StreamSocketAcceptorServer : public ISocketServer
{
public:
	StreamSocketAcceptorServer(const socket_address& addr, const std::function<void(int)>& onConnect, unsigned numAcceptors=1);
	~StreamSocketAcceptorServer();

	bool start();
	bool stop();

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint);

	bool running() const;
	std::string lastError() const;

protected:
	void run();
	bool fatalError(const std::string& error);

protected:
	Socket _sock;
	bool _running;
	socket_address _addr;

	std::function<void(int)> _onConnect;
	unsigned _numAcceptors;
	std::deque<std::thread> _acceptors;
	std::string _lastError;
};

template <typename Socket>
StreamSocketAcceptorServer<Socket>::StreamSocketAcceptorServer(const socket_address& addr, const std::function<void(int)>& onConnect, unsigned numAcceptors)
	: _running(false)
	, _addr(addr)
	, _onConnect(onConnect)
	, _numAcceptors(numAcceptors)
{
}

template <typename Socket>
StreamSocketAcceptorServer<Socket>::~StreamSocketAcceptorServer()
{
	stop();
}

template <typename Socket>
bool StreamSocketAcceptorServer<Socket>::start()
{
	if (_running)
		return true;

	_running = true;
	if (!_sock.good())
		return fatalError("couldn't get good socket");

	if (!_sock.bind(_addr))
	{
		_sock.close();
		return fatalError("couldn't bind socket to " + _addr.toString());
	}

	if (!_sock.listen(20))
	{
		_sock.close();
		return fatalError("listen failed on socket " + _addr.toString());
	}

	for (unsigned i = 0; i < _numAcceptors; ++i)
		_acceptors.push_back( std::thread(std::bind(&StreamSocketAcceptorServer<Socket>::run, this)) );
	return _running;
}

template <typename Socket>
bool StreamSocketAcceptorServer<Socket>::stop()
{
	if (!_running)
		return false;

	_running = false;
	_sock.shutdown();

	for (auto it = _acceptors.begin(); it != _acceptors.end(); ++it)
	{
		 if (it->joinable())
			it->join();
	}
	_acceptors.clear();
	return true;
}

template <typename Socket>
void StreamSocketAcceptorServer<Socket>::run()
{
	Socket connection;
	while (_running && (connection = _sock.accept()).good())
	{
		_onConnect(connection.handle());
		connection.close();
	}
}

template <typename Socket>
bool StreamSocketAcceptorServer<Socket>::running() const
{
	return _running;
}

template <typename Socket>
std::shared_ptr<ISocketWriter> StreamSocketAcceptorServer<Socket>::getWriter(const socket_address& endpoint)
{
	return NULL;
}

template <typename Socket>
std::string StreamSocketAcceptorServer<Socket>::lastError() const
{
	return _lastError;
}

template <typename Socket>
bool StreamSocketAcceptorServer<Socket>::fatalError(const std::string& error)
{
	_lastError = error;
	return _running = false;
}
