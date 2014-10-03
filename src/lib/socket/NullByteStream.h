/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IByteStream.h"

class NullByteStream : public IByteStream
{
public:
	unsigned maxPacketLength() const
	{
		return 2000;
	}

	int read(char* buffer, unsigned length)
	{
		return 0;
	}

	int write(const char* buffer, unsigned length)
	{
		return 0;
	}
};

