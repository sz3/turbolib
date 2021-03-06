/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "HttpParser.h"
#include "util/CallHistory.h"
#include <iostream>
using std::string;

TEST_CASE( "HttpParserTest/testParseRequest", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnMessageBegin( [&] () {
		history.call("begin");
		return 0;
	} );
	parser.setOnMessageComplete( [&] () {
		history.call("complete");
		return 0;
	} );

	parser.setOnUrl( [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len));
		return 0;
	} );

	parser.setOnHeaderField( [&] (const char* buff, size_t len) {
		history.call("onHeaderField", string(buff,len));
		return 0;
	} );
	parser.setOnHeaderValue( [&] (const char* buff, size_t len) {
		history.call("onHeaderValue", string(buff,len));
		return 0;
	} );
	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.method());
		return 0;
	} );

	parser.setOnStatus( [&] (const char* buff, size_t len) {
		history.call("onStatus", string(buff,len));
		return 0;
	} );
	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	parser.parseBuffer("POST /index.html HTTP/1.1\r\n");
	parser.parseBuffer("host: gotham.com\r\n");
	parser.parseBuffer("content-length: 10\r\n\r\n");
	parser.parseBuffer("0123456789");

	assertEquals("begin()|onUrl(/index.html)"
				 "|onHeaderField(host)|onHeaderValue(gotham.com)"
				 "|onHeaderField(content-length)|onHeaderValue(10)"
				 "|onHeadersComplete(3)"
				 "|onBody(0123456789)"
				 "|complete()", history.calls());
}

TEST_CASE( "HttpParserTest/testParseRequest.Chunked", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnUrl( [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len));
		return 0;
	} );

	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	parser.parseBuffer("POST /index.html HTTP/1.1\r\n");
	parser.parseBuffer("transfer-encoding: chunked\r\n\r\n");
	parser.parseBuffer("4\r\n");
	parser.parseBuffer("0123\r\n");
	parser.parseBuffer("6\r\n");
	parser.parseBuffer("456789\r\n");
	parser.parseBuffer("0\r\n\r\n");

	assertEquals("onUrl(/index.html)|onBody(0123)|onBody(456789)", history.calls());
}

TEST_CASE( "HttpParserTest/testParseRequest.ResumeUrl", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnUrl( [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len));
		return 0;
	} );
	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.method());
		return 0;
	} );
	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	parser.parseBuffer("GET /whoa/this/is/really/a/very");
	parser.parseBuffer("long/url/index.html HTTP/1.1\r\n");
	parser.parseBuffer("\r\n");

	assertEquals("onUrl(/whoa/this/is/really/a/very)|onUrl(long/url/index.html)|onHeadersComplete(1)", history.calls());
}

TEST_CASE( "HttpParserTest/testParseRequest.Bad", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnMessageBegin( [&] () {
		history.call("begin");
		return 0;
	} );
	parser.setOnMessageComplete( [&] () {
		history.call("complete");
		return 0;
	} );

	parser.setOnUrl( [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len));
		return 0;
	} );
	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.method());
		return 0;
	} );
	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	assertFalse( parser.parseBuffer("PUT HTTP/Foo.Bar 200 OK\r\n\r\n") );
	assertEquals("invalid URL", parser.lastError());
	assertEquals("begin()", history.calls());
}

TEST_CASE( "HttpParserTest/testParseResponse", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnMessageBegin( [&] () {
		history.call("begin");
		return 0;
	} );
	parser.setOnMessageComplete( [&] () {
		history.call("complete");
		return 0;
	} );

	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.code());
		return 0;
	} );
	parser.setOnStatus( [&] (const char* buff, size_t len) {
		history.call("onStatus", string(buff,len));
		return 0;
	} );

	assertTrue( parser.parseBuffer("HTTP/1.1 204 No body\r\n\r\n") );

	assertEquals("begin()|onStatus(No body)"
				 "|onHeadersComplete(204)"
				 "|complete()", history.calls());
}

TEST_CASE( "HttpParserTest/testParseResponse.Body", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnMessageBegin( [&] () {
		history.call("begin");
		return 0;
	} );
	parser.setOnMessageComplete( [&] () {
		history.call("complete");
		return 0;
	} );

	parser.setOnUrl( [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len));
		return 0;
	} );

	parser.setOnHeaderField( [&] (const char* buff, size_t len) {
		history.call("onHeaderField", string(buff,len));
		return 0;
	} );
	parser.setOnHeaderValue( [&] (const char* buff, size_t len) {
		history.call("onHeaderValue", string(buff,len));
		return 0;
	} );
	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.code());
		return 0;
	} );

	parser.setOnStatus( [&] (const char* buff, size_t len) {
		history.call("onStatus", string(buff,len));
		return 0;
	} );
	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	parser.parseBuffer("HTTP/1.1 200 Success\r\n");
	parser.parseBuffer("Host: gotham.com\r\n");
	parser.parseBuffer("content-length: 10\r\n\r\n");
	parser.parseBuffer("0123456789");

	assertEquals("begin()|onStatus(Success)"
				 "|onHeaderField(Host)|onHeaderValue(gotham.com)"
				 "|onHeaderField(content-length)|onHeaderValue(10)"
				 "|onHeadersComplete(200)"
				 "|onBody(0123456789)"
				 "|complete()", history.calls());
}

TEST_CASE( "HttpParserTest/testParseResponse.Chunked", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	parser.setOnMessageBegin( [&] () {
		history.call("begin");
		return 0;
	} );
	parser.setOnMessageComplete( [&] () {
		history.call("complete");
		return 0;
	} );

	parser.setOnHeadersComplete( [&] (HttpParser::Status status) {
		history.call("onHeadersComplete", status.code());
		return 0;
	} );
	parser.setOnStatus( [&] (const char* buff, size_t len) {
		history.call("onStatus", string(buff,len));
		return 0;
	} );
	parser.setOnBody( [&] (const char* buff, size_t len) {
		history.call("onBody", string(buff,len));
		return 0;
	} );

	parser.parseBuffer("HTTP/1.1 200 OK\r\n");
	parser.parseBuffer("Transfer-encoding: chunked\r\n\r\n");
	parser.parseBuffer("A\r\n");
	parser.parseBuffer("0123456789\r\n");
	parser.parseBuffer("3\r\n");
	parser.parseBuffer("xyz\r\n");
	parser.parseBuffer("0\r\n\r\n");

	assertEquals("begin()|onStatus(OK)"
				 "|onHeadersComplete(200)"
				 "|onBody(0123456789)"
				 "|onBody(xyz)"
				 "|complete()", history.calls());
}
