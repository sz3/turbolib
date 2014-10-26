/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "socket/ISocketWriter.h"
#include "socket/socket_address.h"
#include "util/CallHistory.h"

class MockSocketWriter : public ISocketWriter
{
public:
	MockSocketWriter()
		: _trySendError(false)
		, _trySendErrorBytes(-1)
	{
	}

	int try_send(const char* buffer, unsigned size)
	{
		_history.call("try_send", std::string(buffer, size));
		if (_trySendError)
			return _trySendErrorBytes;
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

	socket_address endpoint() const
	{
		_history.call("endpoint");
		return _endpoint;
	}

	std::string target() const
	{
		_history.call("target");
		return _endpoint.address();
	}

public:
	socket_address _endpoint;
	bool _trySendError;
	int _trySendErrorBytes;

	mutable CallHistory _history;
};
