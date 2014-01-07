/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

class IByteStream
{
public:
	virtual ~IByteStream() {}

	virtual unsigned maxPacketLength() const = 0;

	virtual int read(char* buffer, unsigned length) = 0;
	virtual int write(const char* buffer, unsigned length) = 0;
};

