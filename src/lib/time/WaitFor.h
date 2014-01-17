/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "Timer.h"
#include <exception>
#include <iostream>

class WaitFor
{
public:
	template <typename Funct>
	WaitFor(unsigned seconds, Funct fun)
	{
		Timer t;
		while (1)
		{
			try {
				fun();
				break;
			} catch (...) {
				if (t.millis() >= seconds*1000)
					std::rethrow_exception(std::current_exception());
				CommandLine::run("sleep 5");
			}
		}
	}
};
