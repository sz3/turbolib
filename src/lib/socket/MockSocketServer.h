/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "ISocketServer.h"
#include "ISocketWriter.h"
#include "socket_address.h"
#include "util/CallHistory.h"

class MockSocketServer : public ISocketServer
{
public:
	bool start()
	{
		_history.call("start");
		return true;
	}

	bool stop()
	{
		_history.call("stop");
		return true;
	}

	std::shared_ptr<ISocketWriter> getWriter(const socket_address& endpoint)
	{
		_history.call("getWriter", endpoint.toString());
		return _sock;
	}

	void waitForWriter(int id)
	{
		_history.call("waitForWriter", id);
	}

	std::string lastError() const
	{
		return _error;
	}

public:
	CallHistory _history;
	std::shared_ptr<ISocketWriter> _sock;
	std::string _error;
};
