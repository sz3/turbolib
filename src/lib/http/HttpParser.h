/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>
#include <string>

// TODO: this class arguably only should be treated as static,
//   due to its current implementation of std::function -> c style function pointer conversion code.
class HttpParser
{
public:
	using callback = std::function<int()>;
	using dataCallback = std::function<int(const char*, size_t)>;
public:
	HttpParser();
	~HttpParser();

	bool parseBuffer(const std::string& buffer);
	bool parseBuffer(const char* buffer, unsigned size);

	void setOnMessageBegin(callback fun);
	void setOnMessageComplete(callback fun);

	void setOnUrl(dataCallback fun);
	void setOnHeaderField(dataCallback fun);
	void setOnHeaderValue(dataCallback fun);
	void setOnHeadersComplete(callback fun);
	void setOnStatus(dataCallback fun);
	void setOnBody(dataCallback fun);

protected:
	class Impl;
	Impl* _pimpl;
};

