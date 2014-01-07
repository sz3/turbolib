/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IByteStream.h"
#include <string>

class StringByteStream : public IByteStream
{
public:
	StringByteStream(unsigned packetLength=1024);
	StringByteStream(const std::string& str, unsigned packetLength=1024);

	unsigned maxPacketLength() const;

	int read(char* buffer, unsigned length);
	int write(const char* buffer, unsigned length);

	const std::string& readBuffer() const;
	const std::string& writeBuffer() const;

protected:
	unsigned _packetLength = 1024;
	std::string _readBuffer;
	std::string _writeBuffer;
};
