/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <map>
#include <string>
#include <vector>

class Url
{
public:
	Url(const std::string& url);
	void append(const std::string& token);

	std::string str() const;
	std::string baseUrl() const;
	std::vector<std::string> components() const;
	std::string queryString() const;
	std::map<std::string,std::string> params() const;

protected:
	std::string _url;
};
