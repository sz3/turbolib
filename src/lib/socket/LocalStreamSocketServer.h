#pragma once

#include "event/Event.h"
#include <list>
#include <string>
#include <thread>

class LocalStreamSocketServer
{
public:
	// some sort of packet handling interface
	LocalStreamSocketServer(const std::string& name, unsigned numThreads=1);
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
	bool _running;
	std::string _name;
	unsigned _numThreads;

	std::list<std::thread> _threads;
	int _sock;
	std::string _lastError;
};
