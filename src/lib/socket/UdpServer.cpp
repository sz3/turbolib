#include "UdpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

UdpServer::UdpServer()
	: _running(false)
	, _port(8487)
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

	_waitForRunning.set();

	struct sockaddr_in si_other;
	socklen_t slen = sizeof(si_other);
	const int buflen = 1024;
	char buf[buflen];
	while (_running)
	{
		if (recvfrom(sock, buf, buflen, 0, (struct sockaddr*)&si_other, &slen)==-1)
			continue;

		printf("Received packet from %s:%d\nData: %s\n\n",
			   inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
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
	_waitForRunning.set();
}
