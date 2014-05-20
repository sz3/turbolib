/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "HttpResponse.h"

#include <functional>
using std::bind;
using std::string;
using namespace std::placeholders;

HttpResponse::HttpResponse()
{
	_parser.setOnBody( std::bind(&HttpResponse::appendBody, this, _1, _2) );
	_parser.setOnHeadersComplete( std::bind(&HttpResponse::setStatus, this, _1) );
}

HttpResponse& HttpResponse::parse(const std::string& msg)
{
	// TODO: do something with errors.
	_parser.parseBuffer(msg.data(), msg.size());
	return *this;
}

StatusCode HttpResponse::status() const
{
	return _status;
}

std::string HttpResponse::body() const
{
	return _body;
}

int HttpResponse::setStatus(HttpParser::Status status)
{
	_status = status.code();
	return 0;
}

int HttpResponse::appendBody(const char* data, unsigned size)
{
	_body += string(data, size);
	return 0;
}
