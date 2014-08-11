/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "udt_socket.h"

#include "socket/IpAddress.h"
#include "udt/udt.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <cstring>

namespace {
	void setAsyncWrites(int sock, bool async)
	{
		bool blocking = !async;
		UDT::setsockopt(sock, 0, UDT_SNDSYN, &blocking, sizeof(bool));
	}

	class UdtAsyncOff
	{
	public:
		UdtAsyncOff(UDTSOCKET sock)
			: _sock(sock)
		{
			setAsyncWrites(_sock, false);
		}

		~UdtAsyncOff()
		{
			setAsyncWrites(_sock, true);
		}

	protected:
		UDTSOCKET _sock;
	};
}

udt_socket::udt_socket()
	: udt_socket(UDT::socket(AF_INET, SOCK_DGRAM, 0))
{
}

udt_socket::udt_socket(int sock)
	: _sock(sock)
{
}

udt_socket::udt_socket(const IpAddress& endpoint)
	: udt_socket()
{
	if (good())
		connect(endpoint);
}

int udt_socket::handle() const
{
	return _sock;
}

bool udt_socket::good() const
{
	return _sock != UDT::INVALID_SOCK;
}

IpAddress udt_socket::endpoint() const
{
	struct sockaddr_in endpoint;
	int size = sizeof(endpoint);
	UDT::getpeername(_sock, (sockaddr*)&endpoint, &size);
	return IpAddress(inet_ntoa(endpoint.sin_addr), ntohs(endpoint.sin_port));
}

bool udt_socket::connect(const IpAddress& endpoint)
{
	struct sockaddr_in stemp;
	stemp.sin_family = AF_INET;
	stemp.sin_port = htons(endpoint.port());
	inet_aton(endpoint.ip().c_str(), &stemp.sin_addr);

	int res = UDT::connect(_sock, (const sockaddr*)&stemp, sizeof(stemp));
	if (res == UDT::ERROR)
		_sock = UDT::INVALID_SOCK;
	else
		setAsyncWrites(_sock, true);
	return res == 0;
}

bool udt_socket::bind(short port)
{
	struct sockaddr_in bind_in;
	memset((char*)&bind_in, 0, sizeof(bind_in));

	bind_in.sin_family = AF_INET;
	bind_in.sin_port = htons(port);
	bind_in.sin_addr.s_addr = htonl(INADDR_ANY);
	return UDT::bind(_sock, (struct sockaddr*)&bind_in, sizeof(bind_in)) != UDT::ERROR;
}

bool udt_socket::listen(int seconds)
{
	return UDT::listen(_sock, seconds) != UDT::ERROR;
}

udt_socket udt_socket::accept()
{
	struct sockaddr_in their_addr;
	int addr_sz = sizeof(their_addr);
	memset((char*)&their_addr, 0, addr_sz);

	UDTSOCKET conn = UDT::accept(_sock, (sockaddr*)&their_addr, &addr_sz);
	if (conn != UDT::INVALID_SOCK)
		setAsyncWrites(conn, true);
	return udt_socket(conn);
}

bool udt_socket::close()
{
	UDT::close(_sock);
	// TODO: use return code, yo
	return true;
}

bool udt_socket::shutdown()
{
	return false;
}

int udt_socket::try_send(const char* buffer, unsigned size)
{
	return UDT::sendmsg(_sock, buffer, size, -1, true);
}

int udt_socket::try_send(const std::string& buffer)
{
	return try_send(buffer.data(), buffer.size());
}

int udt_socket::send(const char* buffer, unsigned size)
{
	UdtAsyncOff sync(_sock);
	return try_send(buffer, size);
}

int udt_socket::send(const std::string& data)
{
	return send(data.data(), data.size());
}

int udt_socket::recv(char* buffer, unsigned size)
{
	int bytes = UDT::recvmsg(_sock, buffer, size);
	if (bytes < 0)
		std::cout << "UdtSocket::recv crashed and burned: " << UDT::getlasterror().getErrorMessage() << std::endl;
	return bytes;
}

std::string udt_socket::getErrorMessage() const
{
	return UDT::getlasterror().getErrorMessage();
}
