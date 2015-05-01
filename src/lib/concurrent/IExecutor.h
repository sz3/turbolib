/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>

class IExecutor
{
public:
	virtual ~IExecutor() {}

	virtual void execute(std::function<void()> fun) = 0;

	virtual bool start() { return true; }
	virtual void stop() {}
};
