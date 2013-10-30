#pragma once

#include <functional>
#include <thread>

class Executor
{
public:
	Executor() {}

	template <typename Func>
	void run(Func fun)
	{
		std::thread(fun).detach();
	}

protected:
// maybe a legit threadpool eventually.
};
