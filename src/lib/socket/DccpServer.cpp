#include "DccpServer.h"

#include "DccpSocket.h"
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

DccpServer::DccpServer(short port, std::function<void(const IIpSocket& sock, const std::string&)> onPacket, unsigned maxPacketSize)
	: _running(false)
	, _sock(-1)
	, _port(port)
	, _maxPacketSize(maxPacketSize)
	, _onPacket(onPacket)
{
}

DccpServer::~DccpServer()
{
	stop();
}

bool DccpServer::start()
{
	_running = true;
	_sock = ::socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP); // different than UDP server
	if (_sock == -1)
	{
		fatalError("couldn't create socket!");
		return _running = false;
	}

	struct sockaddr_in si_me;
	memset((char*)&si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(_port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind(_sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		fatalError("bind() failed for DCCP socket");
		::close(_sock);
		return _running = false;
	}
	if(::listen(_sock, 20) != 0)
	{
		fatalError("listen() failed for DCCP socket");
		::close(_sock);
		return _running = false;
	}

	_acceptor = std::thread( std::bind(&DccpServer::accept, this) );
	//_runner = std::thread( std::bind(&DccpServer::run, this) );
	_started.wait();
	return _running;
}

void DccpServer::stop()
{
	_running = false;
	::shutdown(_sock, SHUT_RDWR);
	if (_runner.joinable())
		_runner.join();
	if (_acceptor.joinable())
		_acceptor.join();
}

// TODO: probably eventually want two epoll groups.
// one for new connections,
// and one for known (good) peers.
// that way we can safely do a LRU scheme for good peers...
void DccpServer::accept()
{
	_started.shutdown();

	struct sockaddr_in their_addr;
	socklen_t addr_sz = sizeof(their_addr);
	memset((char*)&their_addr, 0, addr_sz);

	while (_running)
	{
		int conn = ::accept(_sock, (sockaddr*)&their_addr, &addr_sz);
		if (conn < 0)
			continue;

		std::string buffer;
		buffer.resize(_maxPacketSize);

		DccpSocket sock(conn);
		if (sock.recv(buffer) > 0)
			_onPacket(sock, buffer);

		::close(conn);
	}
}

void DccpServer::run()
{

}

bool DccpServer::isRunning() const
{
	return _running;
}

std::string DccpServer::lastError() const
{
	return _lastError;
}

void DccpServer::fatalError(const std::string& error)
{
	_lastError = error;
}

std::shared_ptr<IIpSocket> DccpServer::sock(const IpAddress& addr)
{
	int clientSock = ::socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	DccpSocket* res = new DccpSocket(clientSock);
	std::shared_ptr<IIpSocket> thesock(res);
	if (!res->connect(addr))
		return NULL;
	return thesock;
}
