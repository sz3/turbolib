#include "UdtServer.h"

#include "UdtSocket.h"
#include <iostream>

#include "udt4/src/udt.h"
#include <arpa/inet.h>
//#include <netinet/in.h>
#include <string.h>
//#include <unistd.h>

namespace
{
	int _epollEventFlag = EPOLLOpt::UDT_EPOLL_IN ^ EPOLLOpt::UDT_EPOLL_ERR;
}

// TODO: investigate delays/unreliabiliy when using concurrent UDT::epoll_wait(), e.g. numThreads > 1?
UdtServer::UdtServer(short port, std::function<void(const IIpSocket&, const std::string&)> onPacket, unsigned numThreads/*=1*/, unsigned maxPacketSize)
	: _running(false)
	, _sock(-1)
	, _port(port)
	, _numThreads(numThreads)
	, _maxPacketSize(maxPacketSize)
	, _onPacket(onPacket)
{
}

UdtServer::~UdtServer()
{
	stop();
}

bool UdtServer::start()
{
	if (_running)
		return true;

	_running = true;
	_sock = UDT::socket(AF_INET, SOCK_DGRAM, 0);
	if (_sock == UDT::INVALID_SOCK)
	{
		fatalError("couldn't create socket!");
		return _running = false;
	}

	struct sockaddr_in si_me;
	memset((char*)&si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(_port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (UDT::bind(_sock, (struct sockaddr*)&si_me, sizeof(si_me)) == UDT::ERROR)
	{
		fatalError("couldn't bind to port: " + std::string(UDT::getlasterror().getErrorMessage()));
		return _running = false;
	}
	if (UDT::listen(_sock, 20) == UDT::ERROR)
	{
		fatalError("couldn't get udt socket to listen: " + std::string(UDT::getlasterror().getErrorMessage()));
		UDT::close(_sock);
		return _running = false;
	}

	_pollPackets = UDT::epoll_create();
	if (_pollPackets < 0)
	{
		fatalError("couldn't get udt epoll to work: " + std::string(UDT::getlasterror().getErrorMessage()));
		UDT::close(_sock);
		return _running = false;
	}

	_acceptor = std::thread( std::bind(&UdtServer::accept, this) );
	for (unsigned i = 0; i < _numThreads; ++i)
		_runners.push_back( std::thread(std::bind(&UdtServer::run, this)) );
	_started.wait();
	return _running;
}

void UdtServer::stop()
{
	_running = false;
	UDT::epoll_release(_pollPackets);
	UDT::close(_sock);
	for (tbb::concurrent_unordered_map<std::string,int>::iterator it = _connections.begin(); it != _connections.end(); ++it)
		UDT::close(it->second);

	for (std::list<std::thread>::iterator it = _runners.begin(); it != _runners.end(); ++it)
	{
		 if (it->joinable())
			it->join();
	}
	_runners.clear();
	if (_acceptor.joinable())
		_acceptor.join();
}

// TODO: probably eventually want two epoll groups.
// one for new connections,
// and one for known (good) peers.
// that way we can safely do a LRU scheme for good peers...
void UdtServer::accept()
{
	_started.shutdown();

	struct sockaddr_in their_addr;
	int addr_sz = sizeof(their_addr);
	memset((char*)&their_addr, 0, addr_sz);

	while (_running)
	{
		UDTSOCKET conn = UDT::accept(_sock, (sockaddr*)&their_addr, &addr_sz);
		if (conn == UDT::INVALID_SOCK)
			continue;

		if (UDT::epoll_add_usock(_pollPackets, conn, &_epollEventFlag) < 0)
		{
			std::cout << "UDT::epoll_add_usock error." << std::endl;
			continue;
		}
	}
}

void UdtServer::run()
{
	std::string buffer;
	std::set<UDTSOCKET> irrelevant;
	std::set<UDTSOCKET> reads;
	while (_running)
	{
		reads.clear();
		irrelevant.clear();

		int waiters;
		if ((waiters = UDT::epoll_wait(_pollPackets, &reads, &irrelevant, 5000)) < 0)
		{
			std::cout << "epoll_wait did an error! :( " << waiters << std::endl;
			continue;
		}
		std::cout << "epoll says there are " << waiters << " guys ready. Yes, " << reads.size() << " ... ? " << irrelevant.size() << std::endl;

		for (std::set<UDTSOCKET>::const_iterator it = reads.begin(); it != reads.end(); ++it)
		{
			UdtSocket sock(*it);
			std::cout << "udt read: " << sock.getTarget().toString() << std::endl;

			buffer.resize(_maxPacketSize);
			if (sock.recv(buffer) <= 0)
			{
				std::cout << "badness. :(" << std::endl;
				UDT::epoll_remove_usock(_pollPackets, *it);
				UDT::close(*it);
			}

			_onPacket(sock, buffer);
			_connections[sock.getTarget().toString()] = *it;
		}
	}
}

bool UdtServer::isRunning() const
{
	return _running;
}

std::string UdtServer::lastError() const
{
	return _lastError;
}

void UdtServer::fatalError(const std::string& error)
{
	_lastError = error;
}

std::shared_ptr<IIpSocket> UdtServer::sock(const IpAddress& addr)
{
	// concurrent unordered map or something
	std::shared_ptr<IIpSocket> res;
	std::pair< tbb::concurrent_unordered_map<std::string, int>::iterator, bool> pear = _connections.insert( {addr.toString(), -1} );
	if (pear.first->second < 0)
	{
		UDTSOCKET handle = UDT::socket(AF_INET, SOCK_DGRAM, 0);
		if (handle == UDT::INVALID_SOCK)
			return NULL;

		UdtSocket* sock = new UdtSocket(handle);
		res.reset(sock);
		if (!sock->connect(addr))
			return NULL;
		pear.first->second = handle;

		if (UDT::epoll_add_usock(_pollPackets, handle, &_epollEventFlag) < 0)
			std::cout << "UDT::epoll_add_usock error for client sock." << std::endl;
	}
	else
		res.reset(new UdtSocket(pear.first->second));
	return std::shared_ptr<IIpSocket>(res);
}
