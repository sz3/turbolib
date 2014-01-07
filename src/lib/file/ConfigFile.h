/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <map>
#include <string>

class ConfigFile
{
public:
	ConfigFile(std::string filename);

	bool load();
	bool save() const;

	std::string get(const std::string& option) const;
	void set(const std::string& option, const std::string& value);

protected:
	void loadLine(const std::string& line);

protected:
	std::string _filename;
	std::map<std::string,std::string> _config;
};
