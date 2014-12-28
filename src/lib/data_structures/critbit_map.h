/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#include "critbit_tree.h"

#include <utility>

template <class Key, class Value>
struct critbit_map_pair : std::pair<Key, Value>
{
	using std::pair<Key,Value>::first;
	using std::pair<Key,Value>::second;

	// constructors
	using std::pair<Key,Value>::pair;

	critbit_map_pair(const Key& key)
		: std::pair<Key,Value>(key, Value())
	{}

	operator const uint8_t*() const
	{
		return (const uint8_t*)&first;
	}

	bool operator==(const critbit_map_pair& right) const
	{
		return first == right.first;
	}

	size_t key_size() const
	{
		return sizeof(Key);
	}
};

// critbit_int_map?
template < typename Key, typename Value, typename Node=critbit_node, typename Pair=critbit_map_pair<Key,Value> >
class critbit_map
{
public:
	class elem
	{
	public:
		elem(Pair* val)
			: _val(val)
		{
		}

		operator bool() const
		{
			return _val != NULL;
		}

		const Key& first() const
		{
			return _val->first;
		}

		Key& first()
		{
			return _val->first;
		}

		const Value& second() const
		{
			return _val->second;
		}

		Value& second()
		{
			return _val->second;
		}

	protected:
		Pair* _val;
	};

public:
	int insert(const Pair&& pair)
	{
		return _tree.insert(pair);
	}

	int remove(const Key& key)
	{
		return _tree.remove(key);
	}

	elem find(const Key& key) const
	{
		return _tree.find(key);
	}

	elem lower_bound(const Key& key) const
	{
		return _tree.lower_bound(key);
	}

	bool contains(const Key& key) const
	{
		return _tree.contains(key);
	}

	bool empty() const
	{
		return _tree.empty();
	}

	void clear()
	{
		_tree.clear();
	}

protected:
	critbit_tree<Pair, const Pair&, Node> _tree;
};

