/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "stopwatch.h"
#include <chrono>
#include <functional>
#include <regex>
#include <string>
#include <thread>

#define wait_for(s,msg,fun) assertMsg(turbo::WaitFor(s,fun).result(), msg)

namespace turbo {

class WaitFor
{
public:
	template <typename Funct>
	WaitFor(unsigned seconds, Funct fun)
	{
		_result = false;
		stopwatch t;
		while (1)
		{
			_result = fun();
			if (_result)
				break;
			if (t.millis() >= seconds*1000)
				break;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	bool result() const
	{
		return _result;
	}

protected:
	bool _result;
};

inline void wait_for_equal(unsigned seconds, std::string expected, std::function<std::string()> fun)
{
	std::string result;
	wait_for(seconds, expected + " != " + result, [&]()
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
