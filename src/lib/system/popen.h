/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <cstdio>
#include <string>

namespace turbo {
class popen
{
public:
	popen()
		: _stdout(NULL)
	{}

	popen(const std::string& command)
		: popen()
	{
		open(command);
	}

	~popen()
	{
		if ( good() )
			::pclose(_stdout);
	}

	bool open(const std::string& command)
	{
		if ( good() )
			return false;

		_stdout = ::popen(command.c_str(), "r");
		return good();
	}

	std::string read()
	{
		std::string output;
		if ( !good() )
			return output;

		char buffer[1024];
		while (::fgets(buffer, sizeof(buffer), _stdout) != NULL)
			output += buffer;
		return output;
	}

	bool good() const
	{
		return _stdout != NULL;
	}

protected:
	FILE* _stdout;
};
} // namespace turbo
