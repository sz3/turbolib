/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.h"
#include <string>
#include <utility>

class FileRemover
{
public:
	FileRemover(std::string filename)
		: _filename(std::move(filename))
	{}

	~FileRemover()
	{
		File::remove(_filename);
	}

protected:
	std::string _filename;
};
