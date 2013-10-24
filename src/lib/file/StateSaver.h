#pragma once

#include "File.h"
#include <fstream>
#include <functional>
#include <sstream> // for now

class StateSaver
{
public:
	StateSaver(const std::string& filename)
		: _filename(filename)
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
