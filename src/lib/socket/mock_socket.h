#pragma once

#include "socket_address.h"
#include "util/CallHistory.h"
#include <memory>
#include <string>

class mock_socket
{
public:
	mock_socket(std::string endpoint)
		: _history(new CallHistory())
		, _endpoint(endpoint)
		, _trySendError(false)
		, _trySendErrorBytes(-1)
	{}

	std::string target() const
	{
		return _endpoint.toString();
	}

	socket_address endpoint() const
	{
		return _endpoint;
	}

	int handle() const
	{
		return 4;
	}

	int try_send(const char* buffer, unsigned size)
	{
		_history->call("try_send", std::string(buffer, size));
		if (_trySendError)
			return _trySendErrorBytes;
		return size;
	}

	int send(const char* buffer, unsigned size)
	{
		_history->call("send", std::string(buffer, size));
		return size;
	}

	bool close()
	{
		_history->call("close");
		return true;
	}

	CallHistory& history()
	{
		return *_history;
	}

public:
	std::shared_ptr<CallHistory> _history;
	socket_address _endpoint;

	bool _trySendError;
	int _trySendErrorBytes;
};
