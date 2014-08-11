/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

class ISocketWriter
{
public:
	virtual ~ISocketWriter() {}

	virtual int try_send(const char* buffer, unsigned size) = 0;
	virtual int send(const char* buffer, unsigned size) = 0;
};
