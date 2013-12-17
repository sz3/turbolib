#include "FileByteStream.h"

#include <cstdio> // for perror
#include <sys/socket.h>

FileByteStream::FileByteStream(int fd)
	: _fd(fd)
{
}

unsigned FileByteStream::maxPacketLength() const
{
	return 8192;
}

int FileByteStream::read(char* buffer, unsigned length)
{
	// TODO: _lastError here.
	int nbytes = ::recv(_fd, buffer, length, 0);
	if (nbytes < 0)
		perror("FileByteStream::read");
	return nbytes;
}

int FileByteStream::write(const char* buffer, unsigned length)
{
	int nbytes = ::send(_fd, buffer, length, 0);
	if (nbytes < 0)
		perror("send");
	return nbytes;
}