/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>

class IScheduler
{
public:
	virtual ~IScheduler() {}

	virtual void execute(const std::function<void()>& fun) = 0;
	virtual void schedule(const std::function<void()>& fun, unsigned ms) = 0;
	virtual void schedule_repeat(const std::function<void()>& fun, unsigned ms) = 0;
};

