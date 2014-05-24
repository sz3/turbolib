/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "HttpParser.h"
#include "StatusCode.h"

// TODO: sort out the HttpParser static thread_local problem...
class HttpResponse
{
public:
	HttpResponse();

	HttpResponse& parse(const std::string& msg);
	HttpResponse& parse(const char* msg, unsigned length);

	StatusCode status() const;
	std::string body() const;

protected:
	int setStatus(HttpParser::Status status);
	int appendBody(const char* data, unsigned size);

protected:
	HttpParser _parser;

	StatusCode _status;
	std::string _body;
};
