#include "Timer.h"

using namespace std::chrono;

Timer::Timer()
{
	reset();
}

void Timer::reset()
{
	_time = high_resolution_clock::now();
}

high_resolution_clock::duration Timer::duration() const
{
	return high_resolution_clock::now() - _time;
}

long long Timer::nanos() const
{
	return duration_cast<nanoseconds>( duration() ).count();
}

long long Timer::micros() const
{
	return duration_cast<microseconds>( duration() ).count();
}

long long Timer::millis() const
{
	return duration_cast<milliseconds>( duration() ).count();
}
