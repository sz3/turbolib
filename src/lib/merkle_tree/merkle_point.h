/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <deque>

namespace turbo {

template <typename KeyType, typename HashType>
struct merkle_point
{
	HashType hash;
	KeyType key;
	uint16_t keybits;

	static constexpr uint16_t MAX_KEYBITS = sizeof(KeyType)*8;

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
		, keybits(MAX_KEYBITS)
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

template <typename KeyType, typename HashType>
class merkle_diff
{
private:
	using ptype = merkle_point<KeyType, HashType>;
	using listype = std::deque<ptype>;

	inline bool leaf() const
	{
		return _points.size() == 1;
	}

public:
	merkle_diff(const listype& points, bool missing=false)
		: _points(points)
		, _missing(missing)
	{}

	bool no_difference() const
	{
		return _points.empty();
	}

	bool traverse() const
	{
		return _points.size() >= 2;
	}

	bool need_range() const
	{
		return _missing && leaf() && _points.front() == ptype::null();
	}

	bool need_partial_range() const
	{
		return _missing && leaf() && !(_points.front() == ptype::null());
	}

	bool need_exchange() const
	{
		return !_missing && leaf();
	}

	std::deque< merkle_point<KeyType, HashType> >& points() const
	{
		return _points;
	}

	size_t size() const
	{
		return _points.size();
	}

	const ptype& operator[](unsigned i) const
	{
		return _points[i];
	}

protected:
	listype _points;
	bool _missing;
};

} // namespace turbo
