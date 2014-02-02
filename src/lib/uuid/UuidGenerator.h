/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

class UuidGenerator
{
public:
	std::string generate()
	{
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		std::stringstream ss;
		ss << uuid;
		return ss.str();
	}
};

