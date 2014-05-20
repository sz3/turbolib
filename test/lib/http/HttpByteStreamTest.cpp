/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "HttpByteStream.h"
#include "socket/StringByteStream.h"
using std::string;

TEST_CASE( "HttpByteStreamTest/testReadForwarding", "[unit]" )
{
	StringByteStream source("0123456789");
	HttpByteStream stream(source);

	string buff;
	buff.resize(10);
	assertEquals( 10, stream.read(&buff[0], buff.size()) );
	assertEquals( "0123456789", buff );
}

TEST_CASE( "HttpByteStreamTest/testWrite", "[unit]" )
{
	StringByteStream dest;
	HttpByteStream stream(dest);

	string buff("0123456789abcd");
	assertEquals( 14, stream.write(buff.data(), buff.size()) );
	assertEquals( "HTTP/1.1 200 Success\r\n"
				  "transfer-encoding: chunked\r\n\r\n"
				  "e\r\n"
				  "0123456789abcd\r\n", dest.writeBuffer() );

	buff = "foo";
	assertEquals( 3, stream.write(buff.data(), buff.size()) );
	assertEquals( "HTTP/1.1 200 Success\r\n"
				  "transfer-encoding: chunked\r\n\r\n"
				  "e\r\n"
				  "0123456789abcd\r\n"
				  "3\r\n"
				  "foo\r\n", dest.writeBuffer() );

	buff = "";
	assertEquals( 0, stream.write(buff.data(), buff.size()) );
	assertEquals( "HTTP/1.1 200 Success\r\n"
				  "transfer-encoding: chunked\r\n\r\n"
				  "e\r\n"
				  "0123456789abcd\r\n"
				  "3\r\n"
				  "foo\r\n"
				  "0\r\n\r\n", dest.writeBuffer() );
}

TEST_CASE( "HttpByteStreamTest/testWriteNothing", "[unit]" )
{
	StringByteStream dest;
	HttpByteStream stream(dest);

	string buff;
	assertEquals( 0, stream.write(buff.data(), buff.size()) );
	assertEquals( "HTTP/1.1 200 Success\r\n"
				  "transfer-encoding: chunked\r\n\r\n"
				  "0\r\n\r\n", dest.writeBuffer() );
}

