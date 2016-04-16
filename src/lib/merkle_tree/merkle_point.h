/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

namespace turbo {

template <typename KeyType, typename HashType>
struct merkle_point
{
	HashType hash;
	KeyType key;
	uint16_t keybits;

	static merkle_point<KeyType,HashType> null()
	{
		static merkle_point<KeyType,HashType> nullStatic = [] ()
		{
			merkle_point<KeyType,HashType> temp(0, KeyType(0), ~0);
			return temp;
		}();
		return nullStatic;
	}

	merkle_point() {}

	merkle_point(const HashType& hash, const KeyType& key)
		: hash(hash)
		, key(key)
		, keybits(sizeof(KeyType)*8)
	{}

	merkle_point(const HashType& hash, const KeyType& key, uint16_t keybits)
		: hash(hash)
		, key(key)
		, keybits(keybits)
	{}

	bool operator==(const merkle_point<KeyType,HashType>& other) const
	{
		return key == other.key && keybits == other.keybits && hash == other.hash;
	}

	merkle_point<KeyType,HashType> copy(const HashType& newhash) const
	{
		return merkle_point<KeyType,HashType>(newhash, key, keybits);
	}
};

} // namespace turbo
