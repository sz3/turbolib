#pragma once

#include "event/Event.h"
#include <functional>
#include <string>
#include <thread>

class UdpSocket;

class UdpServer
{
public:
	UdpServer(short port, std::function<void(UdpSocket&, std::string&)> onPacket);
	~UdpServer();

	bool start();
	void stop();

	void run();
	bool isRunning() const;
	std::string lastError() const;

protected:
	void fatalError(const std::string& error);

protected:
	short _port;
	std::function<void(UdpSocket&, std::string&)> _onPacket;

	Event _waitForRunning;
	std::thread  _thread;
	bool _running;
	std::string _lastError;
};

