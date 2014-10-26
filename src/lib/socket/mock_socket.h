#pragma once

#include "socket_address.h"
#include "util/CallHistory.h"
#include <string>

class mock_socket
{
public:
	mock_socket(std::string endpoint)
		: _endpoint(endpoint)
	{}

	std::string target() const
	{
		return _endpoint.toString();
	}

	socket_address endpoint() const
	{
		return _endpoint;
	}

	int try_send(const char* buffer, unsigned size)
	{
		_history.call("try_send", std::string(buffer, size));
		return size;
	}

	int send(const char* buffer, unsigned size)
	{
		_history.call("send", std::string(buffer, size));
		return size;
	}

	bool close()
	{
		_history.call("close");
		return true;
	}

public:
	CallHistory _history;
	socket_address _endpoint;
};
