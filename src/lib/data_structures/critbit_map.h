#include "critbit_tree.h"

#include <utility>

template <class Key, class Value>
struct critbit_map_pair : std::pair<Key, Value>
{
	// constructor delegation. Thank you, C++11
	using std::pair<Key,Value>::pair;

	using std::pair<Key,Value>::first;
	using std::pair<Key,Value>::second;

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
template < typename Key, typename Pair, typename Node=critbit_node >
class critbit_map_internal
{
public:
	int insert(const Pair& pair)
	{
		return _tree.insert(pair);
	}

	int remove(const Key& key)
	{
		return _tree.remove(key);
	}

	Pair* lower_bound(const Key& key) const
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

template<class Key, class Value, typename Node=critbit_node>
using critbit_map = critbit_map_internal<Key, critbit_map_pair<Key,Value>, Node>;
