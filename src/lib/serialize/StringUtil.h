/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace StringUtil
{
	template <class Iter>
	std::string join(const Iter& start, const Iter& end, char delim=' ')
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
	std::string join(const Type& container, char delim=' ')
	{
		return join(container.begin(), container.end(), delim);
	}

	std::vector<std::string> split(const std::string& input, char delim=' ', bool ignoreEmpty=false);

	template <typename Type>
	std::string str(const Type& val)
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}

	template <>
	inline std::string str<bool>(const bool& val)
	{
		return val? "true" : "false";
	}

	template <typename Integer>
	std::string hexStr(const Integer& number)
	{
		std::stringstream ss;
		ss << std::hex << number;
		return ss.str();
	}

	template <typename T>
	inline bool fromStr(T& var, const std::string& str)
	{
		if (str.empty())
			return false;
		std::stringstream ss(str);
		ss >> var;
		return !!ss;
	}

}
