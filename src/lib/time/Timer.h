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
