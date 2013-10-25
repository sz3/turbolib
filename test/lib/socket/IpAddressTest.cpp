#include "unittest.h"

#include "IpAddress.h"

TEST_CASE( "IpAddressTest/testConstructors", "default" )
{
	{
		IpAddress ip("1.2.3.4", 5);
		assertEquals( "1.2.3.4", ip.ip() );
		assertEquals( 5, ip.port() );
		assertEquals( "1.2.3.4:5", ip.toString() );
	}
	{
		IpAddress ip;
		assertEquals( "", ip.ip() );
		assertEquals( 0, ip.port() );
		assertEquals( ":0", ip.toString() );
	}
}


TEST_CASE( "IpAddressTest/testFromString", "default" )
{
	IpAddress ip;
	assertTrue( ip.fromString("1.2.3.4:5") );
	assertEquals( "1.2.3.4", ip.ip() );
	assertEquals( 5, ip.port() );

	assertTrue( ip.fromString("127.0.0.1:9000") );
	assertEquals( "127.0.0.1", ip.ip() );
	assertEquals( 9000, ip.port() );

	assertFalse( ip.fromString("") );
	assertFalse( ip.fromString("1.2.3.4") );
	assertFalse( ip.fromString("1.2.3.4:") );
	assertEquals( "127.0.0.1", ip.ip() );
	assertEquals( 9000, ip.port() );

	assertTrue( ip.fromString(":10") );
	assertEquals( "", ip.ip() );
	assertEquals( 10, ip.port() );
}

