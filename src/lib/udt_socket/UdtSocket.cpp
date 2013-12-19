#include "UdtSocket.h"

#include "udt4/src/udt.h"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <cstring>

UdtSocket::UdtSocket(int sock)
	: _sock(sock)
{
}

bool UdtSocket::connect(const IpAddress& address)
{
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(address.port());
	inet_aton(address.ip().c_str(), &target.sin_addr);

	int res = UDT::connect(_sock, (const sockaddr*)&target, sizeof(target));
	if (res == UDT::ERROR)
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

int UdtSocket::send(const std::string& data) const
{
	return send(data.data(), data.size());
}

int UdtSocket::send(const char* buffer, unsigned size) const
{
	return UDT::sendmsg(_sock, buffer, size, -1, true);
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
