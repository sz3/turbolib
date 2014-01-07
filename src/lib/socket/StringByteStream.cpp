/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "StringByteStream.h"

#include <cstring>

StringByteStream::StringByteStream(unsigned packetLength)
	: _packetLength(packetLength)
{
}

StringByteStream::StringByteStream(const std::string& readBuffer, unsigned packetLength)
	: _packetLength(packetLength)
	, _readBuffer(readBuffer)
{
}

unsigned StringByteStream::maxPacketLength() const
{
	return _packetLength;
}

int StringByteStream::read(char* buffer, unsigned length)
{
	unsigned bytes = std::min(_readBuffer.size(), length);
	if (bytes == 0)
		return 0;

	strncpy(buffer, _readBuffer.data(), bytes);
	_readBuffer = _readBuffer.substr(bytes);
	return bytes;
}

int StringByteStream::write(const char* buffer, unsigned length)
{
	_writeBuffer.append( std::string(buffer,length) );
	return length;
}

const std::string& StringByteStream::readBuffer() const
{
	return _readBuffer;
}

const std::string& StringByteStream::writeBuffer() const
{
	return _writeBuffer;
}
