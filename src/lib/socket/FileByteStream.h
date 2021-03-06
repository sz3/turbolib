/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IByteStream.h"

class FileByteStream : public IByteStream
{
public:
	FileByteStream(int fd);

	unsigned maxPacketLength() const;

	int read(char* buffer, unsigned length);
	int write(const char* buffer, unsigned length);

	int close();

protected:
	int _fd;
};
