/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <cstring>

static int strnncmp(const char* left, size_t leftLen, const char* right, size_t rightLen)
{
	int res = strncmp(left, right, std::min(leftLen, rightLen));
	if (res != 0)
		return res;
	if (leftLen == rightLen)
		return 0;
	else if (leftLen < rightLen)
		return -1;
	else
		return 1;
}
