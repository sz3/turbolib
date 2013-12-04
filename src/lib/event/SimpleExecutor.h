#pragma once

#include "IExecutor.h"
#include <functional>

// it says simple and it means it!
class SimpleExecutor : public IExecutor
{
public:
	void execute(std::function<void()> fun)
	{
		fun();
	}

protected:
};
