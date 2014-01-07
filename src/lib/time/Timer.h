/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <chrono>

class Timer
{
public:
	Timer();

	void reset();

	std::chrono::high_resolution_clock::duration duration() const;
	long long nanos() const;
	long long micros() const;
	long long millis() const;

protected:
	std::chrono::high_resolution_clock::time_point _time;
};
