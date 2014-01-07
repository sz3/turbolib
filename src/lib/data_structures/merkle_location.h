/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

template <typename KeyType>
struct merkle_location
{
	KeyType key;
	unsigned short keybits;

	merkle_location() {}

	merkle_location(const KeyType& key)
		: key(key)
		, keybits(sizeof(KeyType)*8)
	{}

	merkle_location(const KeyType& key, unsigned short keybits)
		: key(key)
		, keybits(keybits)
	{}
};

template <typename KeyType, typename HashType>
struct merkle_point
{
	merkle_location<KeyType> location;
	HashType hash;

	static merkle_point<KeyType,HashType> null()
	{
		static merkle_point<KeyType,HashType> nullStatic = [] ()
		{
			merkle_point<KeyType,HashType> temp;
			temp.location = merkle_location<KeyType>(KeyType(0), ~0);
			temp.hash = 0;
			return temp;
		}();
		return nullStatic;
	}

	bool operator==(const merkle_point<KeyType,HashType>& other) const
	{
		return location.key == other.location.key
				&& location.keybits == other.location.keybits
				&& hash == other.hash;
	}
};

