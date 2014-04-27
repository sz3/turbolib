/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "StringUtil.h"

std::vector<std::string> StringUtil::split(const std::string& input, char delim/*=' '*/, bool ignoreEmpty/*=false*/)
{
	std::vector<std::string> tokens;

	size_t pos = 0;
	size_t nextPos = 0;
	while ((nextPos = input.find(delim, pos)) != std::string::npos)
	{
		std::string tok = input.substr(pos,nextPos-pos);
		if (!ignoreEmpty || !tok.empty())
			tokens.push_back(tok);
		pos = nextPos+1;
	}

	if (!input.empty())
	{
		std::string tok = input.substr(pos);
		if (!ignoreEmpty || !tok.empty())
			tokens.push_back(tok);
	}

	return tokens;
}
