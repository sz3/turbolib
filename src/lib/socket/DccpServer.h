#pragma once

#include "IPacketServer.h"
#include "event/Event.h"
#include <functional>
#include <string>
#include <thread>

class DccpServer : public IPacketServer
{
public:
	DccpServer(short port, std::function<void(const IIpSocket& sock, const std::string&)> onPacket, unsigned maxPacketSize=1450);
	~DccpServer();

	bool start();
	void stop();

	bool isRunning() const;
	std::string lastError() const;

	std::shared_ptr<IIpSocket> sock(const IpAddress& addr);

protected:
	void accept();
	void run();
	void fatalError(const std::string& error);

protected:
	Event _started;
	bool _running;
	int _sock;

	short _port;
	unsigned _maxPacketSize;
	std::function<void(const IIpSocket& sock, std::string&)> _onPacket;

	std::thread  _acceptor;
	std::thread  _runner;
	std::string _lastError;
};

