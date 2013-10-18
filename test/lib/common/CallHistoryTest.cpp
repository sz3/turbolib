#include "unittest.h"

#include "CallHistory.h"
using std::string;

TEST_CASE( "CallHistoryTest/testCalls", "description" )
{
	CallHistory history;
	history.call("foo");
	history.call("bar", "rab");
	history.call("go", "for", 2, true);
	assertEquals("foo()|bar(rab)|go(for,2,1)", history.calls());

	history.clear();
	assertEquals("", history.calls());
}
