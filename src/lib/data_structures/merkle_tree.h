#pragma once

// re-envisioning merkle tree as such:
// 1) balanced, perhaps not perfectly, but well. red-black tree? What about STL?
// 2) ideally, still obeying some sort of "short-circuit" comparison across the wire? many hashes at once?
// 3) each node in the tree contains a hash of the child nodes!

#include "critbit_map.h"
#include "merkle_location.h"
#include "util/unpack_tuple.h"
#include <deque>

template <typename HashType>
struct merkle_node : public critbit_node
{
	HashType hash;
};

// 'Tuple' will be struct merkle_pair or a subclass.
// see merkle_tree definition below.
template <typename KeyType, typename HashType, typename Tuple>
class critbit_ext< critbit_map_pair<KeyType,Tuple>, merkle_node<HashType> >
{
public:
	void push_change(merkle_node<HashType>* node)
	{
		_changes.push_front(node);
	}

	void onchange()
	{
		for (auto it = _changes.begin(); it != _changes.end(); ++it)
		{
			merkle_node<HashType>* node = *it;
			node->hash = getHash(node->child[0]) ^ getHash(node->child[1]);
			//std::cout << " onchange(" << (unsigned)node->byte << ")! my children's hashes are " << getHash(node->child[0]) << "," << getHash(node->child[1]) << ". Mine is " << node->hash << std::endl;
		}
		_changes.clear();
	}

protected:
	static HashType getHash(void* elem)
	{
		using node_ptr = critbit_node_ptr< critbit_map_pair<KeyType,Tuple>, merkle_node<HashType> >;
		node_ptr node(elem);
		if (node.isLeaf())
			return std::get<0>(node.leaf()->second);
		else
			return node.node()->hash;
	}

protected:
	std::deque<merkle_node<HashType>*> _changes;
};

template <typename KeyType, typename HashType, typename... ValueType>
class merkle_tree : public critbit_map< KeyType, std::tuple<HashType, ValueType...>, merkle_node<HashType> >
{
public:
	using pair = critbit_map_pair<KeyType,std::tuple<HashType, ValueType...>>;

protected:
	using tree_type = critbit_tree< pair, const pair&, merkle_node<HashType> >;
	using map_type = critbit_map< KeyType, std::tuple<HashType, ValueType...>, merkle_node<HashType> >;
	using map_type::_tree;

protected:
	typename tree_type::node_ptr nearest_subtree(const merkle_location<KeyType>& location) const
	{
		unsigned keylen = location.keybits/8;
		unsigned char bitmask = location.keybits%8;
		if (bitmask != 0)
		{
			++keylen;
			bitmask = (1 << (8-bitmask)) - 1;
		}
		return _tree.nearest_subtree(pair(location.key), bitmask, keylen);
	}

	bool getHash(const typename tree_type::node_ptr& node_ptr, HashType& hash) const
	{
		if (node_ptr.isNode())
		{
			merkle_node<HashType>* node = node_ptr.node();
			hash = node->hash;
			return true;
		}

		pair* pear = node_ptr.leaf();
		hash = std::get<0>(pear->second);
		return false;
	}

	merkle_point<KeyType, HashType> getPoint(const KeyType& key, const typename tree_type::node_ptr& node_ptr) const
	{
		merkle_point<KeyType, HashType> point;
		if (node_ptr.isNode())
		{
			merkle_node<HashType>* node = node_ptr.node();
			point.hash = node->hash;
			point.location = merkle_location<KeyType>( key, keybits(node->byte, node->otherbits xor 0xFF)-1 );
		}
		else
		{
			pair* twopull = node_ptr.leaf();
			point.hash = std::get<0>(twopull->second);
			point.location = merkle_location<KeyType>( twopull->first );
		}
		return point;
	}

public:
	static unsigned keybits(uint32_t byte, uint8_t otherbits)
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
	bool hash_lookup(const merkle_location<KeyType>& location, HashType& hash) const
	{
		typename tree_type::node_ptr node_ptr = nearest_subtree(location);
		if (node_ptr.isNull())
		{
			hash = 0;
			return false;
		}
		return getHash(node_ptr, hash);
	}

	merkle_point<KeyType, HashType> top() const
	{
		typename tree_type::node_ptr node_ptr = nearest_subtree(merkle_location<KeyType>(0,0));
		if (node_ptr.isNull())
			return merkle_point<KeyType, HashType>::null();
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
	std::deque< merkle_point<KeyType, HashType> > diff(const merkle_location<KeyType>& location, const HashType& hash) const
	{
		std::deque< merkle_point<KeyType, HashType> > diffs;
		if (_tree.empty())
		{
			diffs.push_back(merkle_point<KeyType, HashType>::null());
			return diffs;
		}

		HashType myhash;
		typename tree_type::node_ptr node_ptr = nearest_subtree(location);
		if (node_ptr.isNull())
		{
			merkle_point<KeyType,HashType> nothing;
			nothing.location = location;
			nothing.hash = 0;
			diffs.push_back(nothing);
			return diffs;
		}

		getHash(node_ptr, myhash);
		if (hash == myhash)
			return diffs; // no differences

		// case 3: leaf
		if (node_ptr.isLeaf())
		{
			merkle_point<KeyType,HashType> current;
			current.location = merkle_location<KeyType>(node_ptr.leaf()->first);
			current.hash = myhash;
			diffs.push_back(current);
			return diffs;
		}

		pair* leftLeaf = _tree.begin(node_ptr);
		merkle_node<HashType>* branchNode = node_ptr.node();
		unsigned branchKeybits = keybits(branchNode->byte, branchNode->otherbits xor 0xFF)-1;

		// case 4: branch, but the location parameter seemed to expect a branch sooner
		if (branchKeybits > location.keybits)
		{
			merkle_point<KeyType,HashType> missing;
			missing.location.key = leftLeaf->first;
			missing.location.keybits = location.keybits;

			unsigned expectedBranchByte = location.keybits / 8;
			unsigned char expectedBranchBits = location.keybits % 8;
			((uint8_t*)&missing.location.key)[expectedBranchByte] ^= (1 << (7-expectedBranchBits));
			diffs.push_back(missing);
			return diffs;
		}

		// case 5,6
		// push children into diffs
		{
			typename tree_type::node_ptr left = branchNode->child[0];
			merkle_point<KeyType, HashType> diff = getPoint(leftLeaf->first, left);
			diffs.push_back(diff);
		}
		{
			typename tree_type::node_ptr right = branchNode->child[1];
			pair* rightLeaf = _tree.begin(right);
			merkle_point<KeyType, HashType> diff = getPoint(rightLeaf->first, right);
			diffs.push_back(diff);
		}

		return diffs;
	}

	int insert(const KeyType& key, const HashType& hash, const ValueType&... value)
	{
		return map_type::insert({key, std::make_tuple(hash, value...)});
	}

	void enumerate(const std::function<bool(const HashType&, const ValueType&...)>& fun, const KeyType& start, const KeyType& finish) const
	{
		auto translator = [&fun] (const pair& pear) {
			return unpack_tuple<sizeof...(ValueType)+1>::unpack(fun, pear.second);
		};
		_tree.enumerate(translator, pair(start), pair(finish));
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
