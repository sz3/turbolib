/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "udt_socket_set.h"

#include "UdtSocket.h"
#include "udt/udt.h"
#include <iostream>

namespace
{
	int _epollReadFlag = EPOLLOpt::UDT_EPOLL_IN ^ EPOLLOpt::UDT_EPOLL_ERR;
	int _epollWriteFlag = EPOLLOpt::UDT_EPOLL_OUT ^ EPOLLOpt::UDT_EPOLL_ERR;
}

udt_socket_set::udt_socket_set(int flags)
	: _pollFlags(flags & WRITES? _epollWriteFlag : _epollReadFlag)
{
	_pollSet = UDT::epoll_create();
	if (_pollSet < 0)
		fatalError("couldn't get udt epoll to work: " + std::string(UDT::getlasterror().getErrorMessage()));
}

udt_socket_set::~udt_socket_set()
{
	release();
}

void udt_socket_set::release()
{
	if (_pollSet >= 0)
	{
		UDT::epoll_release(_pollSet);
		_pollSet = -1;
	}
}

bool udt_socket_set::good() const
{
	return _lastError.empty();
}

std::set<int> udt_socket_set::wait() const
{
	std::set<int> reads;
	std::set<int> writeReady;
	int waiters;
	if ((waiters = UDT::epoll_wait(_pollSet, &reads, &writeReady, 5000)) < 0)
	{
		std::cout << "epoll_wait did an error! :( " << waiters << std::endl;
		return reads;
	}

	if (_pollFlags == _epollWriteFlag)
		return writeReady;
	return reads;
}

bool udt_socket_set::add(int sock)
{
	return UDT::epoll_add_usock(_pollSet, sock, &_pollFlags) >= 0;
}

bool udt_socket_set::remove(int sock)
{
	return UDT::epoll_remove_usock(_pollSet, sock) >= 0;
}

std::string udt_socket_set::lastError() const
{
	return _lastError;
}

void udt_socket_set::fatalError(const std::string& error)
{
	_lastError = error;
}
