#include "UdpServer.h"

#include "UdpSocket.h"
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

UdpServer::UdpServer(short port, std::function<void(UdpSocket&, std::string&)> onPacket)
	: _running(false)
	, _port(port)
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
	_thread = std::thread( std::bind(&UdpServer::run, this) );

	if (!_waitForRunning.wait(5000))
		return false;
	return _running;
}

void UdpServer::stop()
{
	_running = false;
	if (_thread.joinable())
		_thread.join();
}

void UdpServer::run()
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1)
	{
		fatalError("couldn't create socket!");
		return;
	}

	struct sockaddr_in si_me;
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(_port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		fatalError("couldn't bind to port!");
		close(sock);
		return;
	}

	_waitForRunning.signal();

	std::string buffer;
	buffer.resize(1024);
	while (_running)
	{
		UdpSocket udp(sock);
		if (udp.recv(buffer) == -1)
			continue;

		_onPacket(udp, buffer);
		// TODO: when crypto + membership becomes a thing, we'll need to check the client knows what he's doing
		// to avoid DOS. But for now,



	}
	close(sock);
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
	_running = false;
	_waitForRunning.signal();
}
