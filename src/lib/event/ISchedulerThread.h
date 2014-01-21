/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>

class ISchedulerThread
{
public:
	virtual ~ISchedulerThread() {}

	virtual void schedule(const std::function<void()>& fun, unsigned millis) = 0;
	virtual void schedulePeriodic(const std::function<void()>& fun, unsigned millis) = 0;
};
