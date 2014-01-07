/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "File.h"

#include <cstdio>
#include <fstream>
using std::string;

bool File::save(const string& filename, const string& contents)
{
	string tempfile = filename + "~tmp~";
	std::ofstream out(tempfile);
	out << contents;
	out.close();
	if (out.fail())
		return false;

	::remove(filename.c_str());
	return ::rename(tempfile.c_str(), filename.c_str()) == 0;
}

bool File::load(const string& filename, string& contents)
{
	FILE* file = fopen(filename.c_str(), "r");
	if (file == NULL)
		return false;

	char buffer[1024];
	size_t bytesRead = 0;
	while (bytesRead = fread(buffer, 1, 1024, file))
	{
		contents.append(buffer, bytesRead);
		if (bytesRead < 1024)
			break;
	}
	fclose(file);
	return true;
}

bool File::remove(const string& filename)
{
	return ::remove(filename.c_str()) == 0;
}
