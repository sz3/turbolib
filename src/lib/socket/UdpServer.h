#pragma once

#include "IIpSocket.h"
#include <functional>
#include <memory>
#include <string>
#include <thread>

class UdpSocket;

class UdpServer
{
public:
	UdpServer(short port, std::function<void(const IIpSocket&, const std::string&)> onPacket);
	~UdpServer();

	bool start();
	void stop();

	void run();
	bool isRunning() const;
	std::string lastError() const;

	std::shared_ptr<IIpSocket> sock() const;

protected:
	void fatalError(const std::string& error);

protected:
	bool _running;
	int _sock;
	short _port;
	std::function<void(const IIpSocket&, std::string&)> _onPacket;

	std::thread  _thread;
	std::string _lastError;
};

