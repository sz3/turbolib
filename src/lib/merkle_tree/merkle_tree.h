/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "critbit_map.h"
#include "util/unpack_tuple.h"
#include <deque>
#include <functional>

namespace turbo {

template <typename HashType>
struct merkle_branch : public critbit_branch
{
	HashType hash;
};

// 'Tuple' will be struct merkle_pair or a subclass.
// see merkle_tree definition below.
template <typename KeyType, typename HashType, typename Tuple>
class critbit_ext< critbit_map_pair<KeyType,Tuple>, merkle_branch<HashType> >
{
public:
	void push_change(merkle_branch<HashType>* branch)
	{
		_changes.push_front(branch);
	}

	void clear_changes()
	{
		_changes.clear();
	}

	void onchange()
	{
		for (auto it = _changes.begin(); it != _changes.end(); ++it)
		{
			merkle_branch<HashType>& branch = **it;
			branch.hash = getHash(branch.child[0]) ^ getHash(branch.child[1]);
			//std::cout << " onchange(" << (unsigned)branch.byte << ")! my children's hashes are " << getHash(branch.child[0]) << "," << getHash(branch.child[1]) << ". Mine is " << branch.hash << std::endl;
		}
		clear_changes();
	}

protected:
	static HashType getHash(void* elem)
	{
		using node_ptr = critbit_node_ptr< critbit_map_pair<KeyType,Tuple>, merkle_branch<HashType> >;
		node_ptr node(elem);
		if (node.isLeaf())
			return std::get<0>(node.leaf()->second);
		else
			return node.branch().hash;
	}

protected:
	std::deque< merkle_branch<HashType>* > _changes;
};

template <typename KeyType, typename HashType, typename... ValueType>
class merkle_tree : public critbit_map< KeyType, std::tuple<HashType, ValueType...>, merkle_branch<HashType> >
{
protected:
	using internal_pair = critbit_map_pair<KeyType,std::tuple<HashType, ValueType...>>;
	using tree_type = critbit_tree< internal_pair, const internal_pair&, merkle_branch<HashType> >;
	using map_type = critbit_map< KeyType, std::tuple<HashType, ValueType...>, merkle_branch<HashType> >;
	using map_type::_tree;

public:
	using pair = typename map_type::pair;

	struct location
	{
		HashType hash;
		KeyType key;
		uint16_t keybits;

		static constexpr uint16_t MAX_KEYBITS = sizeof(KeyType)*8;

		static location null()
		{
			static location nullStatic = [] ()
			{
				location temp(0, KeyType(0), ~0);
				return temp;
			}();
			return nullStatic;
		}

		location() {}

		location(const HashType& hash, const KeyType& key)
			: hash(hash)
			, key(key)
			, keybits(MAX_KEYBITS)
		{}

		location(const HashType& hash, const KeyType& key, uint16_t keybits)
			: hash(hash)
			, key(key)
			, keybits(keybits)
		{}

		bool operator==(const location& other) const
		{
			return key == other.key && keybits == other.keybits && hash == other.hash;
		}

		location copy(const HashType& newhash) const
		{
			return location(newhash, key, keybits);
		}
	};

	class diff_result
	{
	private:
		using listype = std::deque<location>;

		inline bool leaf() const
		{
			return _points.size() == 1;
		}

	public:
		diff_result(const listype& points, bool missing=false)
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
			return _missing && leaf() && _points.front() == location::null();
		}

		bool need_partial_range() const
		{
			return _missing && leaf() && !(_points.front() == location::null());
		}

		bool need_exchange() const
		{
			return !_missing && leaf();
		}

		std::deque<location>& points()
		{
			return _points;
		}

		size_t size() const
		{
			return _points.size();
		}

		const location& operator[](unsigned i) const
		{
			return _points[i];
		}

	protected:
		listype _points;
		bool _missing;
	};

protected:
	typename tree_type::node_ptr nearest_subtree(KeyType key, uint16_t keybits) const
	{
		unsigned keylen = keybits/8;
		unsigned char bitmask = keybits%8;
		if (bitmask != 0)
		{
			++keylen;
			bitmask = (1 << (8-bitmask)) - 1;
		}
		return _tree.nearest_subtree(internal_pair(key), bitmask, keylen);
	}

	bool getHash(const typename tree_type::node_ptr& node_ptr, HashType& hash) const
	{
		if (node_ptr.isBranch())
		{
			merkle_branch<HashType>& node = node_ptr.branch();
			hash = node.hash;
			return true;
		}

		pair pear( node_ptr.leaf() );
		hash = std::get<0>(pear.second());
		return false;
	}

	location getPoint(const KeyType& key, const typename tree_type::node_ptr& node_ptr) const
	{
		if (node_ptr.isBranch())
		{
			merkle_branch<HashType>& node = node_ptr.branch();
			uint16_t kb = keybits(node.byte, node.otherbits xor 0xFF) - 1;
			return {node.hash, key, kb};
		}
		else
		{
			pair twopull( node_ptr.leaf() );
			return {std::get<0>(twopull.second()), twopull.first()};
		}
	}

public:
	static uint16_t keybits(uint32_t byte, uint8_t otherbits)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightMultLookup
		static const int MultiplyDeBruijnBitPosition[32] =
		{
		  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		return (byte*8) + 8-(MultiplyDeBruijnBitPosition[((uint32_t)((otherbits & -otherbits) * 0x077CB531U)) >> 27]);
	}

	// returns whether true if we found a node, false if we found a leaf
	bool hash_lookup(KeyType key, unsigned short keybits, HashType& hash) const
	{
		typename tree_type::node_ptr node_ptr = nearest_subtree(key, keybits);
		if (node_ptr.isNull())
		{
			hash = 0;
			return false;
		}
		return getHash(node_ptr, hash);
	}

	location top() const
	{
		typename tree_type::node_ptr node_ptr = nearest_subtree(0, 0);
		if (node_ptr.isNull())
			return location::null();
		return getPoint(0, node_ptr);
	}

	/*
	 * diff cases:
	 *  1) empty
	 *  2) no diff
	 *  3) leaf diff -> 1 key is disparate. 1 hash. Need 3rd party to tell us what's wrong.
	 *  4) branch diff, keybits !=. null hashes, just the missing branch. If my keybits > than query, I'm missing information on anything in the range (key ^ branch critbit)
	 *  5) branch diff, parent keybits !=. Detect missing branch! TODO!!!
	 *  6) branch diff, keybits !=. 2 hashes. If my keybits < than query, other party is missing said info
	 *  7) branch diff, keybits ==. 2 hashes. Recurse to right, left sides.
	 */
	diff_result diff(const location& point) const
	{
		std::deque<location> diffs;
		if (_tree.empty())
		{
			diffs.push_back(location::null());
			return {diffs, true};
		}

		typename tree_type::node_ptr nodep = nearest_subtree(point.key, point.keybits);
		if (nodep.isNull())
		{
			diffs.push_back(location::null());
			return {diffs, true};
		}

		HashType myhash;
		getHash(nodep, myhash);
		if (point.hash == myhash)
			return diffs; // no differences

		// case 3: leaf
		if (nodep.isLeaf())
		{
			location current(myhash, nodep.leaf()->first);
			diffs.push_back(current);
			bool is_missing = current.key != point.key || current.keybits != point.keybits;
			return {diffs, is_missing};
		}

		pair leftLeaf( _tree.begin(nodep) );
		merkle_branch<HashType>& branch = nodep.branch();
		uint16_t branchKeybits = keybits(branch.byte, branch.otherbits xor 0xFF)-1;

		// case 4: branch, but the location parameter seemed to expect a branch sooner
		if (branchKeybits > point.keybits)
		{
			location missing;
			missing.hash = branch.hash;
			missing.key = leftLeaf.first();
			missing.keybits = branchKeybits;
			diffs.push_back(missing);
			return {diffs, true};
		}

		// case 5,6
		// push children into diffs
		{
			typename tree_type::node_ptr left = branch.child[0];
			location diff = getPoint(leftLeaf.first(), left);
			diffs.push_back(diff);
		}
		{
			typename tree_type::node_ptr right = branch.child[1];
			pair rightLeaf( _tree.begin(right) );
			location diff = getPoint(rightLeaf.first(), right);
			diffs.push_back(diff);
		}
		// and passed in loc with disagreement hash as 3rd element
		{
			location current = point.copy(myhash);
			diffs.push_back(current);
		}

		return diffs;
	}

	int insert(const KeyType& key, const HashType& hash, const ValueType&... value)
	{
		return map_type::insert({key, std::make_tuple(hash, value...)});
	}

	void enumerate(const std::function<bool(const KeyType&, const HashType&, const ValueType&...)>& fun, const KeyType& start, const KeyType& finish) const
	{
		auto translator = [&fun] (const internal_pair& pear) {
			return unpack_tuple<sizeof...(ValueType)+1>::unpack(fun, pear.first, pear.second);
		};
		_tree.enumerate(translator, internal_pair(start), internal_pair(finish));
	}

	// ping|foo
	// diff -> receive a node and its immediate children?
	// ack|oof 17:bar 18:stew

	// or maybe just a list of expected values (given that stuff can always change, no value is guaranteed to be in sync)
	// ack|17:bar 18:stew

	// when we receive a Key,Hash pair, there are a couple possibilities:
	// 1) we have it, and it's the same. yay!
	// 2) we have it, and it's different. This is the start (root) diff case. We need to send the nodes' children pairs as a response.
	// 3) we don't have it. "I need everything under this node, e.g. every file that begins with this prefix"

	// want to compare/return diff of someone else's state
};

} // namespace turbo
