#pragma once

#include "IByteStream.h"

class ConnectionByteStream : public IByteStream
{
public:
	ConnectionByteStream(int fd);

	int read(char* buffer, unsigned length);
	int write(const char* buffer, unsigned length);

protected:
	int _fd;
};
