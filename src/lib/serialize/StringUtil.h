#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace StringUtil
{
	template <class Iter>
	std::string stlJoin(const Iter& start, const Iter& end, char delim=' ')
	{
		std::stringstream ss;
		Iter it = start;
		if (it != end)
			ss << *it++;
		for (; it != end; ++it)
			ss << delim << *it;
		return ss.str();
	}

	template <class Type>
	std::string stlJoin(const Type& container, char delim=' ')
	{
		return stlJoin(container.begin(), container.end(), delim);
	}

	std::vector<std::string> split(const std::string& input, char delim=' ');

	template <typename Type>
	std::string str(const Type& val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}
