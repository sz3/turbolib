/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>
class socket_address;

class ISocketWriter
{
public:
	virtual ~ISocketWriter() {}

	virtual int try_send(const char* buffer, unsigned size) = 0;
	virtual int send(const char* buffer, unsigned size) = 0;

	// TODO: make this go away
	virtual bool close() = 0;

	virtual socket_address endpoint() const = 0;
	virtual std::string target() const = 0; // TODO: this should go away
};
