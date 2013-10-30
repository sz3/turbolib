#include "LocalStreamSocketServer.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <utility>
using std::string;

// TODO: something to limit size of name. tied to "UNIX_PATH_MAX", which is defined in a mystery location, about ~108 chars
LocalStreamSocketServer::LocalStreamSocketServer(string name, const std::function<void(int)>& onConnect, unsigned numThreads/*=1*/)
	: _running(false)
	, _sock(-1)
	, _name(std::move(name))
	, _onConnect(onConnect)
	, _numThreads(numThreads)
{
}

LocalStreamSocketServer::~LocalStreamSocketServer()
{
	stop();
}

bool LocalStreamSocketServer::start()
{
	if (_running)
		return true;

	_running = true;
	_sock = ::socket(PF_UNIX, SOCK_STREAM, 0); // different than UDP server
	if (_sock == -1)
	{
		fatalError("couldn't create socket!");
		return _running = false;
	}

	// also different. Remove inode for name
	::unlink(_name.c_str());

	//use different type of sockaddr
	struct sockaddr_un si_me;
	memset(&si_me, 0, sizeof(struct sockaddr_un));
	si_me.sun_family = AF_UNIX;
	snprintf(si_me.sun_path, _name.size()+1, _name.c_str());

	if (::bind(_sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		fatalError("couldn't bind to domain socket!");
		::close(_sock);
		return _running = false;
	}

	// stream socket, different behavior after here. (listen, accept, send, recv)
	if(::listen(_sock, 5) != 0)
	{
		fatalError("listen() failed on domain socket");
		::close(_sock);
		return _running = false;
	}

	for (unsigned i = 0; i < _numThreads; ++i)
		_threads.push_back( std::thread(std::bind(&LocalStreamSocketServer::run, this)) );
	return _running;
}

void LocalStreamSocketServer::stop()
{
	_running = false;
	::shutdown(_sock, SHUT_RDWR);
	for (std::list<std::thread>::iterator it = _threads.begin(); it != _threads.end(); ++it)
	{
		 if (it->joinable())
			it->join();
	}
	_threads.clear();
}

// maybe schedule this on multiple threads at once...
void LocalStreamSocketServer::run()
{
	struct sockaddr_un si_other;
	socklen_t slen = sizeof(si_other);

	int connection;
	while (_running && (connection = ::accept(_sock, (struct sockaddr*)&si_other, &slen)) > -1)
	{
		_onConnect(connection);
		close(connection);
	}
	close(_sock);
}

bool LocalStreamSocketServer::isRunning() const
{
	return _running;
}

std::string LocalStreamSocketServer::lastError() const
{
	return _lastError;
}

void LocalStreamSocketServer::fatalError(const std::string& error)
{
	_lastError = error;
}
