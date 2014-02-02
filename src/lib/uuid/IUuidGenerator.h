/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>

class IUuidGenerator
{
public:
	virtual ~IUuidGenerator() {}

	virtual std::string generate() = 0;
};

