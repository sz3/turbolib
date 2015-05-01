/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
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
