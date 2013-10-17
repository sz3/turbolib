#pragma once

class IByteStream
{
public:
	virtual ~IByteStream() {}

	virtual int read(char* buffer, unsigned length) = 0;
	virtual int write(const char* buffer, unsigned length) = 0;
};

