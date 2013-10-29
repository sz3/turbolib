#include "ConnectionByteStream.h"

#include <cstdio> // for perror
#include <sys/socket.h>

ConnectionByteStream::ConnectionByteStream(int fd)
	: _fd(fd)
{
}

unsigned ConnectionByteStream::maxPacketLength() const
{
	return 8192;
}

int ConnectionByteStream::read(char* buffer, unsigned length)
{
	// TODO: _lastError here.
	int nbytes = ::recv(_fd, buffer, length, 0);
	if (nbytes < 0)
		perror("ConnectionByteStream::read");
	return nbytes;
}

int ConnectionByteStream::write(const char* buffer, unsigned length)
{
	int nbytes = ::send(_fd, buffer, length, 0);
	if (nbytes < 0)
		perror("send");
	return nbytes;
}
