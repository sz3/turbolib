/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "File.h"

#include <cstdio>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
using std::string;

namespace {
	bool flush(const string& filename)
	{
		int fd = ::open(filename.c_str(), O_RDONLY);
		if (fd == -1)
			return false;
		if (::fsync(fd) != 0)
			return false;
		::close(fd);	// if this fails, what to do?
		return true;
	}

	string dirname(const string& filename)
	{
		return filename.substr(0, filename.find_last_of("/"));
	}
}

bool File::save(const string& filename, const string& contents)
{
	string tempfile = filename + "~tmp~";
	std::ofstream out(tempfile);
	out << contents;
	out.close();
	if (out.fail())
		return false;

	// must flush manually - close and rename does not imply fsync
	// it would be nice to not reopen the file for this, but
	// ofstream gives no way to get at the file handle.
	if (!flush(tempfile))
		return false;

	if (::rename(tempfile.c_str(), filename.c_str()) != 0)
		return false;

	// must also flush the directory, see man fsync(2)
	return flush(dirname(filename));
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
