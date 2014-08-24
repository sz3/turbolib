/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "socket_address.h"

TEST_CASE( "socket_addressTest/testConstructors", "[unit]" )
{
	{
		socket_address addr("1.2.3.4", 5);
		assertEquals( "1.2.3.4", addr.address() );
		assertEquals( 5, addr.port() );
		assertEquals( "1.2.3.4:5", addr.toString() );
	}
	{
		socket_address addr("localhost", 50000);
		assertEquals( "localhost", addr.address() );
		assertEquals( 50000, addr.port() );
		assertEquals( "localhost:50000", addr.toString() );
	}
	{
		socket_address addr;
		assertEquals( "", addr.address() );
		assertEquals( 0, addr.port() );
		assertEquals( ":0", addr.toString() );
	}
}

TEST_CASE( "socket_addressTest/testFromString", "[unit]" )
{
	socket_address addr;
	assertTrue( addr.fromString("1.2.3.4:5") );
	assertEquals( "1.2.3.4", addr.address() );
	assertEquals( 5, addr.port() );

	assertTrue( addr.fromString("localhost:50000") );
	assertEquals( "localhost", addr.address() );
	assertEquals( 50000, addr.port() );

	assertFalse( addr.fromString("") );
	assertFalse( addr.fromString("1.2.3.4") );
	assertFalse( addr.fromString("1.2.3.4:") );
	assertEquals( "localhost", addr.address() );
	assertEquals( 50000, addr.port() );

	assertTrue( addr.fromString(":10") );
	assertEquals( "", addr.address() );
	assertEquals( 10, addr.port() );

	assertTrue( addr.fromString("::20") );
	assertEquals( ":", addr.address() );
	assertEquals( 20, addr.port() );
}

