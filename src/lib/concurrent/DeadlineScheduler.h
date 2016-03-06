/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IScheduler.h"
#include "concurrent/deadline_thread.h"

class DeadlineScheduler : public IScheduler, public turbo::deadline_thread
{
public:
	void execute(const std::function<void()>& fun)
	{
		schedule(fun, 0);
	}

	void schedule(const std::function<void()>& fun, unsigned ms)
	{
		deadline_thread::schedule(fun, ms);
	}

	void schedule_repeat(const std::function<void()>& fun, unsigned ms)
	{
		deadline_thread::schedule_repeat(fun, ms);
	}
};

