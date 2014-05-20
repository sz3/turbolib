/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "HttpResponse.h"
using std::string;

TEST_CASE( "HttpResponseTest/testDefault", "[unit]" )
{
	string response;
	response = "HTTP/1.1 202 Success\r\n"
			   "Transfer-encoding: chunked\r\n\r\n"
			   "5\r\n"
			   "01234\r\n"
			   "3\r\n"
			   "xyz\r\n"
			   "0\r\n\r\n";

	HttpResponse res;
	res.parse(response);
	assertEquals( 202, res.status().integer() );
	assertEquals( "01234xyz", res.body() );
}

TEST_CASE( "HttpResponseTest/testQuick", "[unit]" )
{
	string response;
	response = "HTTP/1.1 200 Success\r\n"
			"Transfer-encoding: chunked\r\n\r\n"
			"5\r\n"
			"hello\r\n"
			"0\r\n\r\n";

	assertEquals( "hello", HttpResponse().parse(response).body() );
}
