/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "unittest.h"

#include "routing_table.h"
#include <deque>
#include <map>

using kademlia::routing_table;

TEST_CASE( "routing_tableTest/testDefault", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);

	assertTrue( table.insert(4) );

	auto it = table.find(4);
	assertTrue( it != table.end() );
	assertEquals( 4, (*it) );

	assertTrue( table.find(0) == table.end() );
}

TEST_CASE( "routing_tableTest/testInsertAndErase", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);
	assertEquals( 0, table.size() );

	std::deque<unsigned> elems = {4, 25, 1337};
	for (unsigned i : elems)
		assertTrue( table.insert(i) );
	assertEquals( 3, table.size() );

	for (unsigned i : elems)
	{
		auto it = table.find(i);
		assertTrue( it != table.end() );
		assertEquals( i, (*it) );
	}

	for (unsigned i : elems)
	{
		assertTrue( table.erase(i) );
		assertTrue( table.find(i) == table.end() );
	}
	assertEquals( 0, table.size() );
}

TEST_CASE( "routing_tableTest/testIterate", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);

	std::deque<unsigned> elems = {4, 25, 1337};
	for (unsigned i : elems)
		assertTrue( table.insert(i) );

	routing_table<unsigned, std::deque>::const_iterator it = table.begin();

	assertTrue( it != table.end() );
	assertEquals( (*it), 4 );
	++it;

	assertTrue( it != table.end() );
	assertEquals( (*it), 25 );
	++it;

	assertTrue( it != table.end() );
	assertEquals( (*it), 1337 );
	++it;

	assertTrue( it == table.end() );
}

TEST_CASE( "routing_tableTest/testIterate.Again", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);

	std::deque<unsigned> elems = {4, 25, 1337};
	for (unsigned i : elems)
		assertTrue( table.insert(i) );

	routing_table<unsigned, std::deque>::const_iterator it = table.begin();
	for (unsigned i : elems)
	{
		assertTrue( it != table.end() );
		assertEquals( i, (*it) );
		++it;
	}

	assertTrue( it == table.end() );
}

TEST_CASE( "routing_tableTest/testRandom", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);

	{
		routing_table<unsigned, std::deque>::const_iterator it = table.random();
		assertTrue( it == table.end() );
	}

	std::deque<unsigned> elems = {4, 25, 1337};
	for (unsigned i : elems)
		assertTrue( table.insert(i) );

	for (int i = 0; i < 50; ++i)
	{
		routing_table<unsigned, std::deque>::const_iterator it = table.random();
		assertTrue( it != table.end() );
		assertInRange( 4, (*it), 1337 );
	}
}

TEST_CASE( "routing_tableTest/testRandom.Big", "[unit]" )
{
	routing_table< unsigned, std::deque > table;
	table.set_origin(0);

	{
		routing_table<unsigned, std::deque>::const_iterator it = table.random();
		assertTrue( it == table.end() );
	}

	std::deque<unsigned> elems;
	for (unsigned i = 0; i < 1000; ++i)
		assertTrue( table.insert(i) );
	assertEquals( 1000, table.size() );

	for (int i = 0; i < 50; ++i)
	{
		routing_table<unsigned, std::deque>::const_iterator it = table.random();
		assertTrue( it != table.end() );
		assertInRange( 0, (*it), 999 );
	}
}

namespace {
	class Peer
	{
	public:
		Peer(unsigned ip, std::string name)
			: ip(ip)
			, name(name)
		{
		}

		bool operator==(const Peer& other) const
		{
			return ip == other.ip;
		}

		bool operator==(unsigned other) const
		{
			return ip == other;
		}

	public:
		unsigned ip;
		std::string name;
	};

	class Hasher
	{
	public:
		using result_type = size_t;

	public:
		result_type operator()(const std::shared_ptr<Peer>& peer) const
		{
			return peer->ip;
		}

		result_type operator()(const Peer& peer) const
		{
			return peer.ip;
		}

		result_type operator()(unsigned ip) const
		{
			return ip;
		}
	};

	bool operator==(const std::shared_ptr<Peer>& p1, const std::shared_ptr<Peer>& p2)
	{
		if (!p1 or !p2)
			return false;
		return *p1 == *p2;
	}

	bool operator==(const std::shared_ptr<Peer>& p1, const unsigned ip)
	{
		if (!p1)
			return false;
		return *p1 == ip;
	}
}

TEST_CASE( "routing_tableTest/testClass", "[unit]" )
{
	routing_table< Peer, std::deque, Hasher > table;
	table.set_origin(Peer(0, "foo"));

	std::map<unsigned, std::string> elems = {
		{4, "bar"},
		{12, "oof"},
		{72, "cabbage"}
	};

	for (auto pear : elems)
		assertTrue( table.insert(Peer(pear.first, pear.second)) );

	for (auto pear : elems)
	{
		auto it = table.find(pear.first);
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it).ip );
		assertEquals( pear.second, (*it).name );

		Peer peerless(pear.first, pear.second);
		it = table.find(peerless);
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it).ip );
		assertEquals( pear.second, (*it).name );
	}

	auto it = table.begin();
	for (auto pear : elems)
	{
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it).ip );
		assertEquals( pear.second, (*it).name );
		++it;
	}
	assertTrue( it == table.end() );

	for (auto pear : elems)
	{
		assertTrue( table.erase(pear.first) );
		assertTrue( table.find(pear.first) == table.end() );
	}
}

TEST_CASE( "routing_tableTest/testFindAndModify", "[unit]" )
{
	routing_table< Peer, std::deque, Hasher > table;
	table.set_origin(Peer(0, "foo"));

	assertTrue( table.insert(Peer(4, "bar")) );

	{
		auto it = table.find(4);
		assertTrue( it != table.end() );
		assertEquals( "bar", (*it).name );
		(*it).name = "foo";
	}

	{
		auto it = table.find(4);
		assertTrue( it != table.end() );
		assertEquals( "foo", (*it).name );
	}

	assertEquals( 1, table.size() );
}

TEST_CASE( "routing_tableTest/testSharedPtrToClass", "[unit]" )
{
	routing_table< std::shared_ptr<Peer>, std::deque, Hasher > table;
	std::shared_ptr<Peer> origin(new Peer(0, "foo"));
	table.set_origin(origin);

	std::map<unsigned, std::string> elems = {
		{4, "bar"},
		{12, "oof"},
		{72, "cabbage"}
	};

	for (auto pear : elems)
		assertTrue( table.insert(std::shared_ptr<Peer>(new Peer(pear.first, pear.second))) );

	for (auto pear : elems)
	{
		auto it = table.find(pear.first);
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it)->ip );
		assertEquals( pear.second, (*it)->name );

		std::shared_ptr<Peer> peerless = *it;
		it = table.find(peerless);
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it)->ip );
		assertEquals( pear.second, (*it)->name );
	}

	auto it = table.begin();
	for (auto pear : elems)
	{
		assertTrue( it != table.end() );
		assertEquals( pear.first, (*it)->ip );
		assertEquals( pear.second, (*it)->name );
		++it;
	}
	assertTrue( it == table.end() );

	for (auto pear : elems)
	{
		assertTrue( table.erase(pear.first) );
		assertTrue( table.find(pear.first) == table.end() );
	}
}

