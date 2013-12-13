#include "UdpServer.h"

#include "UdpSocket.h"
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

UdpServer::UdpServer(short port, std::function<void(const IIpSocket&, const std::string&)> onPacket, unsigned maxPacketSize)
	: _running(false)
	, _sock(-1)
	, _port(port)
	, _maxPacketSize(maxPacketSize)
	, _onPacket(onPacket)
{
}

UdpServer::~UdpServer()
{
	stop();
}

bool UdpServer::start()
{
	_running = true;
	_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_sock == -1)
	{
		fatalError("couldn't create socket!");
		return _running = false;
	}

	struct sockaddr_in si_me;
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(_port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		fatalError("couldn't bind to port!");
		::close(_sock);
		return _running = false;
	}

	_thread = std::thread( std::bind(&UdpServer::run, this) );
	return _running;
}

void UdpServer::stop()
{
	_running = false;
	::shutdown(_sock, SHUT_RDWR);
	::close(_sock);
	if (_thread.joinable())
		_thread.join();
}

void UdpServer::run()
{
	UdpSocket udp(_sock);
	std::string buffer;
	while (_running)
	{
		buffer.clear();
		buffer.resize(_maxPacketSize);
		if (udp.recv(buffer) <= 0)
			continue;

		_onPacket(udp, buffer);
		// TODO: when crypto + membership becomes a thing, we'll need to check the client knows what he's doing
		// to avoid DOS. But for now, just make sure there's a connection...
	}
}

bool UdpServer::isRunning() const
{
	return _running;
}

std::string UdpServer::lastError() const
{
	return _lastError;
}

void UdpServer::fatalError(const std::string& error)
{
	_lastError = error;
}

std::shared_ptr<IIpSocket> UdpServer::sock() const
{
	return std::shared_ptr<IIpSocket>(new UdpSocket(_sock));
}
