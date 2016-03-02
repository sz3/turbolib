/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "Url.h"

#include "serialize/str.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
using std::string;
using turbo::str::split;

Url::Url(const std::string& url)
	: _url(url)
{
}

// static
std::string Url::encode(const std::string& text)
{
	std::stringstream res;
	for (char c : text)
	{
		switch (c >> 4)
		{
			case 5:  // [P, _]
			case 4:  // [@, O]
				if (c != '@' && c != '[' && c != ']')
					break;
			case 3:  // [0, ?]
				if (c < ':')
					break;
			case 2:  // [space, /]

				res << "%" << std::hex << static_cast<unsigned int>(c);
				continue;
			default:
				break;
		}
		res << c;
	}
	return res.str();
}

std::string Url::decode(const std::string& text)
{
	std::string res;
	for (unsigned i = 0; i < text.size(); ++i)
	{
		if (text[i] == '%' && (i+2 < text.size()))
		{
			std::string temp = text.substr(i+1, 2);
			char val = static_cast<char>(std::strtoul(temp.c_str(), nullptr, 16));
			if (val >= ' ')
			{
				res += val;
				i += 2;
				continue;
			}
		}
		res += text[i];
	}
	return res;
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
	return split(baseUrl(), '/', true);
}

std::string Url::queryString() const
{
	size_t pos = _url.find('?');
	if (pos >= _url.size()-1)
		return "";
	return _url.substr(pos+1);
}

std::unordered_map<std::string,std::string> Url::params() const
{
	std::unordered_map<string,string> params;
	std::vector<std::string> paramPairs = split( queryString(), '&' );
	for (auto it = paramPairs.begin(); it != paramPairs.end(); ++it)
	{
		const string& token = *it;
		size_t equalPos = token.find('=');
		if (equalPos >= token.size()-1)
			params[token] = "";
		else
			params[token.substr(0,equalPos)] = decode(token.substr(equalPos+1));
	}
	return params;
}
