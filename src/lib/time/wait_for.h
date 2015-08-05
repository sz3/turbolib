/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "stopwatch.h"
#include "serialize/str.h"
#include <chrono>
#include <functional>
#include <regex>
#include <string>
#include <thread>

#define wait_for_true(s,msg,fun) assertMsg(turbo::_wait_for_equal(s,true,fun), msg)

#define wait_for_equal(s,expected,fun) \
{ \
	auto wait_res = turbo::_wait_for_equal(s, expected, fun); \
	assertEquals(expected, wait_res); \
}

#define wait_for_match(s,pattern,fun) [&]() \
{ \
	std::string result; \
	std::smatch matches; \
	wait_for_true(s, pattern + " \n doesn't match\n" + result, [&]() \
	{ \
		result = fun(); \
		return std::regex_match(result, matches, std::regex(pattern)); \
	}); \
	if (matches.empty()) \
		return std::string(); \
	return (std::string)matches[matches.size()-1]; \
}()

namespace turbo {

template <typename CompType, typename Funct>
inline CompType _wait_for_equal(unsigned seconds, CompType expected, Funct fun)
{
	CompType result;
	stopwatch t;
	while (1)
	{
		result = fun();
		if (result == expected)
			break;
		if (t.millis() >= seconds*1000)
			break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return result;
}

} // namespace turbo

