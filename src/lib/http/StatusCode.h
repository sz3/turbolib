/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "serialize/str.h"
#include <string>

class StatusCode
{
public:
	enum : int
	{
		Success = 200,
		Created = 201,
		Accepted = 202,
		PartialContent = 203,
		BadRequest = 400,
		FileNotFound = 404,
		NotAcceptable = 406,
		InternalServerError = 500,
	};

public:
	StatusCode(int code=0);
	StatusCode& operator=(int code);
	operator int() const;

	int integer() const;
	std::string str() const;
	std::string verbose() const;

protected:
	int _code;
};

inline StatusCode::StatusCode(int code)
	: _code(code)
{
}

inline StatusCode& StatusCode::operator=(int code)
{
	_code = code;
	return *this;
}

inline StatusCode::operator int() const
{
	return _code;
}

inline int StatusCode::integer() const
{
	return _code;
}

inline std::string StatusCode::str() const
{
	return turbo::str::str(_code);
}

inline std::string StatusCode::verbose() const
{
	switch (_code)
	{
		case Success:
			return "Success";
		case Created:
			return "Created";
		case Accepted:
			return "Accepted";
		case PartialContent:
			return "Partial Content";
		case BadRequest:
			return "Bad Request";
		case FileNotFound:
			return "File Not Found";
		case NotAcceptable:
			return "Not Acceptable";
		case InternalServerError:
		default:
			return "Internal Server Error";
	};
}
