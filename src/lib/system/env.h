#pragma once

#include <cstdio>
#include <map>
#include <string>

namespace turbo {

namespace env
{
	std::string get(const std::string& key)
	{
		char* val = getenv(key.c_str());
		if (val == NULL)
			return "";
		return std::string(val);
	}
}

} // namespace turbo
