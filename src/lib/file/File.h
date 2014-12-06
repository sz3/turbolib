/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <string>

namespace File
{
	bool save(const std::string& filename, const std::string& contents);
	bool load(const std::string& filename, std::string& contents);
	bool remove(const std::string& filename);
	bool rename(const std::string& src, const std::string& dest);
	bool exists(const std::string& filename);
	unsigned long long size(const std::string& filename);
}

