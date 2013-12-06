#pragma once

#include "IByteStream.h"

class FileByteStream : public IByteStream
{
public:
	FileByteStream(int fd);

	unsigned maxPacketLength() const;

	int read(char* buffer, unsigned length);
	int write(const char* buffer, unsigned length);

protected:
	int _fd;
};
