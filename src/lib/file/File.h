/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>

namespace File
{
	bool save(const std::string& filename, const std::string& contents);
	bool load(const std::string& filename, std::string& contents);
	bool remove(const std::string& filename);
}

