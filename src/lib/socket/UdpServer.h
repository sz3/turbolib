#pragma once

#include "event/Event.h"
#include <string>
#include <thread>

class UdpServer
{
public:
	UdpServer(short port); // IPacketHandler&
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

	Event _waitForRunning;
	std::thread  _thread;
	bool _running;
	std::string _lastError;
};

