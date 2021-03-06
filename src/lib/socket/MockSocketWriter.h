/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "socket/ISocketWriter.h"
#include "socket/socket_address.h"
#include "util/CallHistory.h"

namespace {
	std::string boolStr(bool val)
	{
		if (val)
			return "true";
		else
			return "false";
	}
}

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
		return _capacity;
	}

	bool flush(bool wait)
	{
		_history.call("flush", boolStr(wait));
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

	int handle() const
	{
		_history.call("handle");
		return 3;
	}

	void set_muxid(unsigned char id)
	{
		_history.call("set_muxid", (int)id);
	}

public:
	socket_address _endpoint;
	bool _trySendError;
	int _trySendErrorBytes;
	unsigned _capacity;

	mutable CallHistory _history;
};
