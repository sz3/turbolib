/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

// inherit from map, overriding all iterator calls with circular iterator.
// do nothing else.

#include "circular_iterator.h"
#include <map>

namespace turbo {
template <typename KeyType, typename ValueType>
class circular_map : public std::map<KeyType, ValueType>
{
public:
	using map_type = std::map<KeyType, ValueType>;
	using circular_iterator = turbo::circular_iterator<typename std::map<KeyType, ValueType>::const_iterator>;

public:
	using typename map_type::map; // constructors

	circular_iterator find(const KeyType& key) const
	{
		return circular_iterator(map_type::find(key), map_type::begin(), map_type::end());
	}

	circular_iterator lower_bound(const KeyType& key) const
	{
		typename map_type::const_iterator it = map_type::lower_bound(key);
		if (it == map_type::end())
			it = map_type::begin();
		return circular_iterator(it, map_type::begin(), map_type::end());
	}

	circular_iterator upper_bound(const KeyType& key) const
	{
		typename map_type::const_iterator it = map_type::upper_bound(key);
		if (it == map_type::end())
			it = map_type::begin();
		return circular_iterator(it, map_type::begin(), map_type::end());
	}
};
} // namespace turbo
