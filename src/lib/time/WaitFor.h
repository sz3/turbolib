/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "Timer.h"
#include "command_line/CommandLine.h"
#include <exception>
#include <iostream>

#define waitFor(s,msg,fun) assertMsg(WaitFor(s,fun).result(), msg)

class WaitFor
{
public:
	template <typename Funct>
	WaitFor(unsigned seconds, Funct fun)
	{
		_result = false;
		Timer t;
		while (1)
		{
			_result = fun();
			if (_result)
				break;
			if (t.millis() >= seconds*1000)
				break;
			CommandLine::run("sleep 5");
		}
	}

	bool result() const
	{
		return _result;
	}

protected:
	bool _result;
};
