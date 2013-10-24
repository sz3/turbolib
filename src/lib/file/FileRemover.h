#pragma once

#include "File.h"
#include <string>

class FileRemover
{
public:
	FileRemover(const std::string& filename)
		: _filename(filename)
	{}

	~FileRemover()
	{
		File::remove(_filename);
	}

protected:
	std::string _filename;
};
