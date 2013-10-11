#include "catch.hpp"

#include "LocalStreamSocketServer.h"

#include <cstdio>
#include <iostream>
using namespace std;

namespace {
	string runCommand(const string& command)
	{
		FILE* file = popen(command.c_str(), "r");
		if ( !file )
			return "";

		char buffer[1024];
		string result = fgets(buffer, sizeof(buffer), file);
		pclose(file);

		return result;
	}
};

TEST_CASE( "LocalStreamSocketServerTest/testDefault", "default" )
{
	LocalStreamSocketServer server("/tmp/iamthebestserver", 4);
	bool res = server.start();
	if (!res)
		cerr << "are things bad? : " << server.lastError() << endl;
	REQUIRE( res == true );

	{
		string response = runCommand("echo 'stfu' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: stfu\n" );
	}

	{
		string response = runCommand("echo 'again' | nc -U /tmp/iamthebestserver");
		REQUIRE( response == "back at you: again\n" );
	}

}

