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
		auto _ = ::mkdtemp(&dirname[0]);

		_temp = dirname;
		auto __ = ::chdir(_temp.c_str());
	}

	~UseTempDirectory()
	{
		auto _ = ::chdir(_original.c_str());
		boost::system::error_code ec;
		boost::filesystem::remove_all(_temp, ec);
	}

	std::string location() const
	{
		return _temp;
	}

protected:
	std::string _original;
	std::string _temp;
};
