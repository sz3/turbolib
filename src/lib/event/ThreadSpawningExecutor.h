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
