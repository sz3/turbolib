/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "socket/IByteStream.h"

class IHttpByteStream : public IByteStream
{
public:
	virtual ~IHttpByteStream() {}

	virtual void setStatus(int code) = 0;
	virtual void reset() = 0;
};

