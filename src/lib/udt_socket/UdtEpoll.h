/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>
#include <string>
class IIpSocket;

class UdtEpoll
{
public:
	UdtEpoll(std::function<void(const IIpSocket&)> onWriteReady);
	~UdtEpoll();

	bool wait_clear();
	bool add(const IIpSocket& sock);

	std::string lastError() const;

protected:
	void fatalError(const std::string& error);

protected:
	std::function<void(const IIpSocket&)> _onWriteReady;
	int _pollSet;

	std::string _lastError;
};
