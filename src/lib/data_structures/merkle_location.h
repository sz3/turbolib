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
};

