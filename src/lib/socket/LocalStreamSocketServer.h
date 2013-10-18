#pragma once

#include "event/Event.h"
#include <functional>
#include <list>
#include <string>
#include <thread>

class LocalStreamSocketServer
{
public:
	// some sort of packet handling interface
	LocalStreamSocketServer(const std::string& name, const std::function<void(int)>& onConnect, unsigned numThreads=1);
	~LocalStreamSocketServer();

	bool start();
	void stop();

	void run();
	bool isRunning() const;
	std::string lastError() const;

protected:
	void fatalError(const std::string& error);

protected:
	bool _running;
	int _sock;
	std::string _name;
	std::function<void(int)> _onConnect;
	unsigned _numThreads;

	std::list<std::thread> _threads;
	std::string _lastError;
};
