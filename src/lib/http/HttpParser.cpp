#include "HttpParser.h"

#include "http_parser/http_parser.h"
#include <memory>

HttpParser::Status::Status(void* parser)
	: _parser(parser)
{
}

unsigned HttpParser::Status::method() const
{
	return static_cast<http_parser*>(_parser)->method;
}

unsigned HttpParser::Status::code() const
{
	return static_cast<http_parser*>(_parser)->status_code;
}

class HttpParser::Impl
{
public:
	Impl()
		: _parser(new http_parser)
		, _settings{0}
	{
		http_parser_init(_parser.get(), HTTP_REQUEST);
	}

	bool parseBuffer(const char* buffer, unsigned size)
	{
		int parsedSize = http_parser_execute(_parser.get(), &_settings, buffer, size);
		return parsedSize == size;
	}

//callbacks
public:
	void setOnMessageBegin(const callback& fun)
	{
		static auto wrapper = fun;
		_settings.on_message_begin = [](http_parser*){ return wrapper(); };
	}

	void setOnMessageComplete(const callback& fun)
	{
		static auto wrapper = fun;
		_settings.on_message_complete = [](http_parser*){ return wrapper(); };
	}

	void setOnUrl(const dataCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_url = [](http_parser*, const char* buff, size_t len){ return wrapper(buff, len); };
	}

	void setOnHeaderField(const dataCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_header_field = [](http_parser*, const char* buff, size_t len){ return wrapper(buff, len); };
	}

	void setOnHeaderValue(const dataCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_header_value = [](http_parser*, const char* buff, size_t len){ return wrapper(buff, len); };
	}

	void setOnHeadersComplete(const infoCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_headers_complete = [](http_parser* parser){ return wrapper(HttpParser::Status(parser)); };
	}

	void setOnStatus(const dataCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_status = [](http_parser*, const char* buff, size_t len){ return wrapper(buff, len); };
	}

	void setOnBody(const dataCallback& fun)
	{
		static auto wrapper = fun;
		_settings.on_body = [](http_parser*, const char* buff, size_t len){ return wrapper(buff, len); };
	}

protected:
	std::unique_ptr<http_parser> _parser;
	http_parser_settings _settings;
};

HttpParser::HttpParser()
	: _pimpl(new Impl)
{
}

HttpParser::~HttpParser()
{
	delete _pimpl;
}

bool HttpParser::parseBuffer(const std::string& buffer)
{
	return _pimpl->parseBuffer(buffer.data(), buffer.size());
}

bool HttpParser::parseBuffer(const char* buffer, unsigned size)
{
	return _pimpl->parseBuffer(buffer, size);
}

void HttpParser::setOnMessageBegin(callback fun)
{
	_pimpl->setOnMessageBegin(fun);
}

void HttpParser::setOnMessageComplete(callback fun)
{
	_pimpl->setOnMessageComplete(fun);
}

void HttpParser::setOnUrl(dataCallback fun)
{
	_pimpl->setOnUrl(fun);
}

void HttpParser::setOnHeaderField(dataCallback fun)
{
	_pimpl->setOnHeaderField(fun);
}

void HttpParser::setOnHeaderValue(dataCallback fun)
{
	_pimpl->setOnHeaderValue(fun);
}

void HttpParser::setOnHeadersComplete(infoCallback fun)
{
	_pimpl->setOnHeadersComplete(fun);
}

void HttpParser::setOnStatus(dataCallback fun)
{
	_pimpl->setOnStatus(fun);
}

void HttpParser::setOnBody(dataCallback fun)
{
	_pimpl->setOnBody(fun);
}
