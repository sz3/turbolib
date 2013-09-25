#pragma once

#include "event/Event.h"
#include <string>
#include <thread>

class LocalStreamSocketServer
{
public:
	LocalStreamSocketServer(const std::string& name);
	~LocalStreamSocketServer();

	bool start();
	void stop();

	void onConnect(int fd);

	void run();
	bool isRunning() const;
	std::string lastError() const;

protected:
	void fatalError(const std::string& error);

protected:
	std::string _name;

	Event _waitForRunning;
	std::thread  _thread;
	bool _running;
	std::string _lastError;
};
