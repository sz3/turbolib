/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "HttpByteStream.h"

#include "serialize/str.h"
using std::string;

HttpByteStream::HttpByteStream(IByteStream& stream)
	: _stream(stream)
	, _status(200)
	, _wroteHeader(false)
{
}

unsigned HttpByteStream::maxPacketLength() const
{
	return 16376; //16384-4-2-2
}

int HttpByteStream::read(char* buffer, unsigned length)
{
	return _stream.read(buffer, length);
}

int HttpByteStream::write(const char* buffer, unsigned length)
{
	if (!_wroteHeader)
		writeHeader();

	string buff = turbo::str::hexStr(length) + "\r\n";
	_stream.write(buff.data(), buff.size());

	int bytesWritten = 0;
	if (length > 0)
		bytesWritten = _stream.write(buffer, length);

	buff = "\r\n";
	_stream.write(buff.data(), buff.size());
	return bytesWritten;
}

// TODO: instead of setStatus, setAttribute?
void HttpByteStream::setStatus(int code)
{
	_status = code;
}

void HttpByteStream::reset()
{
	_wroteHeader = false;
	_status = 200;
}

void HttpByteStream::writeHeader()
{
	_wroteHeader = true;
	string header = "HTTP/1.1 " + _status.str() + " " + _status.verbose() + "\r\n"
			+ "transfer-encoding: chunked\r\n\r\n";
	_stream.write(header.data(), header.size());
}
