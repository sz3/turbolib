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
