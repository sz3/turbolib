/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.h"
#include <fstream>
#include <functional>
#include <sstream> // for now
#include <utility>

class StateSaver
{
public:
	StateSaver(std::string filename)
		: _filename(std::move(filename))
	{}

	template <class IT>
	bool save(IT begin, IT end)
	{
		std::stringstream out;
		for (IT it = begin; it != end; ++it)
			out << *it << '\n';
		return File::save(_filename, out.str());
	}

	bool load(std::function<void(const std::string&)> loadFun)
	{
		std::ifstream in(_filename);
		if (!in.is_open())
			return false;

		std::string line;
		while (std::getline(in, line))
			loadFun(line);
		return true;
	}

protected:
	std::string _filename;
};
