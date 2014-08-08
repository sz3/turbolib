/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "UdtSocket.h"

#include "socket/IpAddress.h"
#include "udt/udt.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <cstring>

namespace {
	class UdtAsyncOff
	{
	public:
		UdtAsyncOff(UDTSOCKET sock)
			: _sock(sock)
		{
			UdtSocket::setAsyncWrites(_sock, false);
		}

		~UdtAsyncOff()
		{
			UdtSocket::setAsyncWrites(_sock, true);
		}

	protected:
		UDTSOCKET _sock;
	};
}

UdtSocket::UdtSocket(int sock)
	: _sock(sock)
{
}

void UdtSocket::setAsyncWrites(int sock, bool async)
{
	bool blocking = !async;
	UDT::setsockopt(sock, 0, UDT_SNDSYN, &blocking, sizeof(bool));
}

bool UdtSocket::connect(const IpAddress& address)
{
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(address.port());
	inet_aton(address.ip().c_str(), &target.sin_addr);

	int res = UDT::connect(_sock, (const sockaddr*)&target, sizeof(target));
	if (res != UDT::ERROR)
		setAsyncWrites(_sock, true);
	else
		std::cout << "UdtSocket::connect crashed and burned: " << UDT::getlasterror().getErrorMessage() << std::endl;

	return res == 0;
}

IpAddress UdtSocket::getTarget() const
{
	struct sockaddr_in target;
	int target_sz = sizeof(target);
	UDT::getpeername(_sock, (sockaddr*)&target, &target_sz);
	return IpAddress(inet_ntoa(target.sin_addr), ntohs(target.sin_port));
}

std::string UdtSocket::destination() const
{
	return getTarget().ip();
}

int UdtSocket::handle() const
{
	return _sock;
}

int UdtSocket::try_send(const char* buffer, unsigned size) const
{
	return UDT::sendmsg(_sock, buffer, size, -1, true);
}

int UdtSocket::send(const char* buffer, unsigned size) const
{
	UdtAsyncOff sync(_sock);
	return try_send(buffer, size);
}

int UdtSocket::send(const std::string& data) const
{
	return send(data.data(), data.size());
}

int UdtSocket::recv(std::string& buffer)
{
	int bytes = UDT::recvmsg(_sock, &buffer[0], buffer.capacity());
	if (bytes >= 0)
		buffer.resize(bytes);
	else
		std::cout << "UdtSocket::recv crashed and burned: " << UDT::getlasterror().getErrorMessage() << std::endl;
	return bytes;
}
