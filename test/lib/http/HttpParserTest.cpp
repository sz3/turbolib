/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "HttpParser.h"
#include "util/CallHistory.h"
#include <iostream>
using std::string;

TEST_CASE( "HttpParserTest/testSimple", "[unit]" )
{
	CallHistory history;
	HttpParser parser;

	auto mbegin = [&] () {
		history.call("begin");
		return 0;
	};
	parser.setOnMessageBegin(mbegin);

	auto onurl = [&] (const char* buff, size_t len) {
		history.call("onUrl", string(buff,len), len );
		return 0;
	};
	parser.setOnUrl(onurl);

	parser.parseBuffer("GET /index.html HTTP/1.1\n");
	parser.parseBuffer("host: gotham.com\n");
	parser.parseBuffer("user-agent: batman\n\n");

	assertEquals("begin()|onUrl(/index.html,11)", history.calls());
}
