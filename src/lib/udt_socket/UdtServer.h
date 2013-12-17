#pragma once

#include "event/Event.h"
#include "socket/IPacketServer.h"
#include <functional>
#include <memory>
#include <string>
#include <thread>

class UdtServer : public IPacketServer
{
public:
	UdtServer(short port, std::function<void(const IIpSocket&, const std::string&)> onPacket, unsigned maxPacketSize=1450);
	~UdtServer();

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
	int _pollPackets;

	short _port;
	unsigned _maxPacketSize;
	std::function<void(const IIpSocket&,std::string&)> _onPacket;

	std::thread  _acceptor;
	std::thread  _runner;
	std::string _lastError;
};

