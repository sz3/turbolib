#include "CommandLine.h"

#include <cstdio>
using std::string;

string CommandLine::run(const string& command)
{
	FILE* file = popen(command.c_str(), "r");
	if ( !file )
		return "";

	char buffer[1024];
	char* result = fgets(buffer, sizeof(buffer), file);
	pclose(file);

	return result == NULL? "" : string(result);
}
