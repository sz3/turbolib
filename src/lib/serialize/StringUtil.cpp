#include "StringUtil.h"

std::vector<std::string> StringUtil::split(const std::string& input, char delim/*=' '*/)
{
	std::vector<std::string> tokens;
	std::stringstream ss(input);
	std::string token;
	while (std::getline(ss, token, delim))
		tokens.push_back(token);
	return tokens;
}
