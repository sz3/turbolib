/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "IExecutor.h"
#include <thread>

class ThreadSpawningExecutor : public IExecutor
{
public:
	void execute(std::function<void()> fun)
	{
		std::thread(fun).detach();
	}

protected:
// maybe a legit threadpool eventually.
};
