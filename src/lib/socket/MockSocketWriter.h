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
		, _capacity(1000)
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

	unsigned capacity() const
	{
		_history.call("capacity");
		return _capacity;
	}

	bool flush(bool wait)
	{
		_history.call("flush", wait);
		return true;
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
	unsigned _capacity;

	mutable CallHistory _history;
};
