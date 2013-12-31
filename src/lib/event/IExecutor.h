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
