/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IHttpByteStream.h"
#include "StatusCode.h"

class HttpByteStream : public IHttpByteStream
{
public:
	HttpByteStream(IByteStream& stream);

	unsigned maxPacketLength() const;

	int read(char* buffer, unsigned length);
	int write(const char* buffer, unsigned length);

	void setStatus(int code);
	void reset();

protected:
	void writeHeader();

protected:
	IByteStream& _stream;
	StatusCode _status;
	bool _wroteHeader;
};

