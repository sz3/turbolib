#include "ConfigFile.h"

#include "File.h"
#include "StateSaver.h"
#include "serialize/StringUtil.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
using std::string;
using namespace std::placeholders;

ConfigFile::ConfigFile(const string& filename)
	: _filename(filename)
{
}

namespace {
	std::ostream& operator<<(std::ostream& outstream, const std::pair<string,string>& val)
	{
		outstream << val.first << "=" << val.second;
		return outstream;
	}
}

void ConfigFile::loadLine(const string& line)
{
	std::vector<string> tokens = StringUtil::split(line, '=');
	if (tokens.size() < 2)
		return;
	set(tokens[0], tokens[1]);
}

bool ConfigFile::load()
{
	StateSaver saver(_filename);
	return saver.load(std::bind(&ConfigFile::loadLine, this, _1));
}

bool ConfigFile::save() const
{
	StateSaver saver(_filename);
	return saver.save(_config.begin(), _config.end());
}

std::string ConfigFile::get(const string& option) const
{
	std::map<string,string>::const_iterator it = _config.find(option);
	if (it == _config.end())
		return "";
	return it->second;
}

void ConfigFile::set(const string& option, const string& value)
{
	_config[option] = value;
}
