/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IExecutor.h"
#include "thread_pool.h"

class ThreadPoolExecutor : public turbo::thread_pool, public IExecutor
{
public:
	void execute(std::function<void()> fun)
	{
		thread_pool::execute(fun);
	}

	bool start()
	{
		return thread_pool::start();
	}

	void stop()
	{
		thread_pool::stop();
	}
};

