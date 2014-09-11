/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <chrono>

class stopwatch
{
public:
	stopwatch()
	{
		reset();
	}

	void reset()
	{
		_time = std::chrono::high_resolution_clock::now();
	}

	long seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>( duration() ).count();
	}

	long long millis() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>( duration() ).count();
	}

	long long micros() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>( duration() ).count();
	}

	long long nanos() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>( duration() ).count();
	}

protected:
	std::chrono::high_resolution_clock::duration duration() const
	{
		return std::chrono::high_resolution_clock::now() - _time;
	}

protected:
	std::chrono::high_resolution_clock::time_point _time;
};
