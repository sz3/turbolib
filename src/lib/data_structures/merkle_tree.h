#pragma once

// re-envisioning merkle tree as such:
// 1) balanced, perhaps not perfectly, but well. red-black tree? What about STL?
// 2) ideally, still obeying some sort of "short-circuit" comparison across the wire? many hashes at once?
// 3) each node in the tree contains a hash of the child nodes!

#include "critbit_map.h"

#include <deque>

template <typename HashType>
struct merkle_node : public critbit_node
{
	HashType hash;
};

template <typename KeyType, typename HashType>
struct merkle_pair : public critbit_map_pair<KeyType,HashType>
{
	using critbit_map_pair<KeyType,HashType>::critbit_map_pair; // constructors
};

template <typename KeyType, typename HashType>
class critbit_ext< merkle_pair<KeyType, HashType>, merkle_node<HashType> >
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
		using node_ptr = critbit_node_ptr< merkle_pair<KeyType, HashType>, merkle_node<HashType> >;
		node_ptr node(elem);
		if (node.isLeaf())
			return node.leaf()->second;
		else
			return node.node()->hash;
	}

protected:
	std::deque<merkle_node<HashType>*> _changes;
};

template <typename KeyType>
struct merkle_location
{
	KeyType key;
	unsigned keybits;

	merkle_location() {}

	merkle_location(const KeyType& key)
		: key(key)
		, keybits(sizeof(KeyType)*8)
	{}

	merkle_location(const KeyType& key, unsigned keybits)
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

template <typename KeyType, typename HashType>
class merkle_tree : public critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >
{
protected:
	using pair = merkle_pair<KeyType,HashType>;
	using tree_type = critbit_tree< pair, const pair&, merkle_node<HashType> >;

	using critbit_map< KeyType, HashType, merkle_node<HashType>, pair >::_tree;

protected:
	typename tree_type::node_ptr lookup(const merkle_location<KeyType>& location) const
	{
		unsigned keylen = location.keybits/8;
		unsigned char bitmask = location.keybits%8;
		if (bitmask > 0)
			++keylen;
		bitmask = (1 << (8-bitmask)) - 1;

		return _tree.subtree(pair(location.key), bitmask, keylen);
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
		hash = pear->second;
		return false;
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
		typename tree_type::node_ptr node_ptr = lookup(location);
		return getHash(node_ptr, hash);
	}

	// match key,keybits,hash against the tree, returning two children if match fails
	std::deque< merkle_point<KeyType, HashType> > diff(const merkle_location<KeyType>& location, const HashType& hash) const
	{
		std::deque< merkle_point<KeyType, HashType> > diffs;

		HashType myhash;
		typename tree_type::node_ptr node_ptr = lookup(location);
		getHash(node_ptr, myhash);
		if (hash == myhash)
			return diffs; // no differences

		if (node_ptr.isLeaf())
		{
			merkle_point<KeyType,HashType> current;
			current.location = merkle_location<KeyType>(node_ptr.leaf()->first, location.keybits);
			current.hash = myhash;
			diffs.push_back(current);
			return diffs;
		}

		pair* childLeaf = _tree.begin(node_ptr);
		merkle_node<HashType>* node = node_ptr.node();

		merkle_location<KeyType> childLoc(childLeaf->first, keybits(node->byte, node->otherbits xor 0xFF));
		// push children into diffs
		{
			typename tree_type::node_ptr left = node->child[0];
			merkle_point<KeyType, HashType> diff;
			getHash(left, diff.hash);
			diff.location = childLoc;
			diffs.push_back(diff);
		}
		{
			typename tree_type::node_ptr right = node->child[1];
			merkle_point<KeyType, HashType> diff;
			getHash(right, diff.hash);
			diff.location = childLoc;
			((uint8_t*)&diff.location.key)[node->byte] ^= (node->otherbits ^ 0xFF);
			diffs.push_back(diff);
		}

		return diffs;
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
