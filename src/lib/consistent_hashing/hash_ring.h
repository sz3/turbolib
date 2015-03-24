/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "circular_map.h"

namespace turbo {
template <typename KeyType, typename ValueType, typename Hash=std::hash<KeyType>>
class hash_ring : public circular_map<typename Hash::result_type, ValueType>
{
public:
	using map_type = circular_map<typename Hash::result_type, ValueType>;
	Hash hash;

public:
	ValueType& operator[](const KeyType& key)
	{
		return map_type::operator[](hash(key));
	}

	size_t remove(const KeyType& key)
	{
		return map_type::erase(hash(key));
	}
};
} // namespace turbo
