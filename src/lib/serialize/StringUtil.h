#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace StringUtil
{
	template <class T>
	std::string stlJoin(const T& start, const T& end, char delim=' ')
	{
		std::stringstream str;
		T it = start;
		if (it != end)
			str << *it++;
		for (; it != end; ++it)
			str << delim << *it;
		return str.str();
	}

	template <class TYPE>
	std::string stlJoin(const TYPE& container, char delim=' ')
	{
		return stlJoin(container.begin(), container.end(), delim);
	}

	std::vector<std::string> split(const std::string& input, char delim=' ');
}
