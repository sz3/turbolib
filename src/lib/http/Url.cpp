/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "Url.h"

#include "serialize/StringUtil.h"
using std::string;
using std::map;

Url::Url(const std::string& url)
	: _url(url)
{
}

void Url::append(const std::string& token)
{
	_url += token;
}

std::string Url::str() const
{
	return _url;
}

std::string Url::baseUrl() const
{
	size_t pos = _url.find('?');
	if (pos == string::npos)
		return _url;
	return _url.substr(0, pos);
}

std::vector<std::string> Url::components() const
{
	return StringUtil::split(baseUrl(), '/', true);
}

std::string Url::queryString() const
{
	size_t pos = _url.find('?');
	if (pos >= _url.size()-1)
		return "";
	return _url.substr(pos+1);
}

std::map<std::string,std::string> Url::params() const
{
	std::map<string,string> params;
	std::vector<std::string> paramPairs = StringUtil::split( queryString(), '&' );
	for (auto it = paramPairs.begin(); it != paramPairs.end(); ++it)
	{
		const string& token = *it;
		size_t equalPos = token.find('=');
		if (equalPos >= token.size()-1)
			params[token] = "";
		else
			params[token.substr(0,equalPos)] = token.substr(equalPos+1);
	}
	return params;
}
