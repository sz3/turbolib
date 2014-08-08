/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "UdtEpoll.h"

#include "UdtSocket.h"
#include "udt/udt.h"
#include <iostream>

namespace
{
	int _epollWriteFlag = EPOLLOpt::UDT_EPOLL_OUT ^ EPOLLOpt::UDT_EPOLL_ERR;
}

UdtEpoll::UdtEpoll(std::function<void(const IIpSocket&)> onWriteReady)
	: _onWriteReady(onWriteReady)
{
	_pollSet = UDT::epoll_create();
	if (_pollSet < 0)
		fatalError("couldn't get udt epoll to work for writes: " + std::string(UDT::getlasterror().getErrorMessage()));
}

UdtEpoll::~UdtEpoll()
{
	UDT::epoll_release(_pollSet);
}

bool UdtEpoll::wait_clear()
{
	std::set<UDTSOCKET> reads;
	std::set<UDTSOCKET> writeReady;
	int waiters;
	if ((waiters = UDT::epoll_wait(_pollSet, &reads, &writeReady, 5000)) < 0)
	{
		std::cout << "epoll_wait for write did an error! :( " << waiters << std::endl;
		return false;
	}

	// pendingSends will be registered in the epoll set, and end up in &writeReady.
	for (std::set<UDTSOCKET>::const_iterator it = writeReady.begin(); it != writeReady.end(); ++it)
	{
		UdtSocket sock(*it);
		UDT::epoll_remove_usock(_pollSet, *it);
		_onWriteReady(sock);
	}
	return true;
}

bool UdtEpoll::add(const IIpSocket& sock)
{
	UDTSOCKET handle = sock.handle();
	if (handle == UDT::INVALID_SOCK)
		return false;

	if (UDT::epoll_add_usock(_pollSet, handle, &_epollWriteFlag) < 0)
		return false;
	return true;
}

std::string UdtEpoll::lastError() const
{
	return _lastError;
}

void UdtEpoll::fatalError(const std::string& error)
{
	_lastError = error;
}
