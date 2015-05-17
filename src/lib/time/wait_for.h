/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "stopwatch.h"
#include "serialize/str.h"
#include <chrono>
#include <functional>
#include <regex>
#include <string>
#include <thread>

namespace turbo {

template <typename Funct>
inline bool _wait_for(unsigned seconds, Funct fun)
{
	bool result = false;
	stopwatch t;
	while (1)
	{
		result = fun();
		if (result)
			break;
		if (t.millis() >= seconds*1000)
			break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return result;
}

template <typename Funct>
inline void wait_for(unsigned seconds, std::string msg, Funct fun)
{
	assertMsg(_wait_for(seconds, fun), msg);
}

template <typename CompType, typename Funct>
inline void wait_for_equal(unsigned seconds, CompType expected, Funct fun)
{
	CompType result;
	wait_for(seconds, str::str(expected) + " != " + str::str(result), [&]()
	{
		result = fun();
		return expected == result;
	});
}

inline std::string wait_for_match(unsigned seconds, std::string pattern, std::function<std::string()> fun)
{
	std::string result;
	std::smatch matches;
	wait_for(seconds, pattern + " != " + result, [&]()
	{
		result = fun();
		return std::regex_match(result, matches, std::regex(pattern));
	});
	if (matches.empty())
		return "";
	return matches[matches.size()-1];
}

} // namespace turbo
