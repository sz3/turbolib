#pragma once

#include <boost/filesystem.hpp>
#include <cstdlib>
#include <unistd.h>

class UseTempDirectory
{
public:
	UseTempDirectory()
		: _original(::get_current_dir_name())
	{
		std::string dirname = "/tmp/turbo_XXXXXX\0";
		::mkdtemp(&dirname[0]);

		_temp = dirname;
		::chdir(_temp.c_str());
	}

	~UseTempDirectory()
	{
		::chdir(_original.c_str());
		boost::filesystem::remove_all(_temp);
	}

	std::string location() const
	{
		return _temp;
	}

protected:
	std::string _original;
	std::string _temp;
};
