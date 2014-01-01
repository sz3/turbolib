#pragma once

#include "IIpSocket.h"
#include "IPacketServer.h"
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <thread>
class IpAddress;
class UdpSocket;

class UdpServer : public IPacketServer
{
public:
	UdpServer(short port, std::function<void(const IIpSocket&, const std::string&)> onPacket, unsigned numThreads=1, unsigned maxPacketSize=1450);
	~UdpServer();

	bool start();
	void stop();

	bool isRunning() const;
	std::string lastError() const;

	std::shared_ptr<IIpSocket> sock(const IpAddress& addr);

protected:
	void run();
	void fatalError(const std::string& error);

protected:
	bool _running;
	int _sock;
	short _port;
	unsigned _numThreads;
	unsigned _maxPacketSize;
	std::function<void(const IIpSocket&, std::string&)> _onPacket;

	std::list<std::thread> _threads;
	std::string _lastError;
};

