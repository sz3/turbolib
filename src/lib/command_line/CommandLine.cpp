/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "CommandLine.h"

#include <cstdio>
using std::string;

string CommandLine::run(const string& command)
{
	FILE* file = popen(command.c_str(), "r");
	if ( !file )
		return "";

	string result;
	char buffer[1024];
	char* bytes = NULL;
	while ((bytes = fgets(buffer, sizeof(buffer), file)) != NULL)
		result += bytes;
	pclose(file);
	return result;
}
