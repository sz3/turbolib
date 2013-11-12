#pragma once

// re-envisioning merkle tree as such:
// 1) balanced, perhaps not perfectly, but well. red-black tree? What about STL?
// 2) ideally, still obeying some sort of "short-circuit" comparison across the wire? many hashes at once?
// 3) each node in the tree contains a hash of the child nodes!

#include "critbit_map.h"

template <typename HashType>
struct merkle_node : public critbit_node
{
	merkle_node* parent;
	HashType hash;
};

template <typename KeyType, typename HashType>
struct merkle_pair : public critbit_map_pair<KeyType,HashType>
{
	using critbit_map_pair<KeyType,HashType>::critbit_map_pair; // constructors

	merkle_node<HashType>* parent;
};

// TODO: probably should go with the "change list", rather than adding parent everywhere.
// pare ourselves down to 1-2 functions instead of 6.
template <typename KeyType, typename HashType>
class critbit_ext< merkle_pair<KeyType, HashType>, merkle_node<HashType> >
{
protected:
	using leaf_type = merkle_pair<KeyType, HashType>;
	using node_ptr = critbit_node_ptr< leaf_type, merkle_node<HashType> >;

public:
	static void assignParent(leaf_type* child, merkle_node<HashType>* parent)
	{
		child->parent = parent;
	}

	static void assignParent(void* child, merkle_node<HashType>* parent)
	{
		node_ptr node(child);
		if (node.isLeaf())
			assignParent(node.leaf(), parent);
		else
			node.node()->parent = parent;
	}

	static void inheritParent(void* successor, merkle_node<HashType>* child)
	{
		assignParent(successor, child->parent);
	}

	static void inheritParent(merkle_node<HashType>* successor, void* child)
	{
		node_ptr node(child);
		if (node.isLeaf())
			successor->parent = node.leaf()->parent;
		else
			successor->parent = node.node()->parent;
	}

	static void onchange(merkle_node<HashType>* node)
	{
		node->hash = getHash(node->child[0]) ^ getHash(node->child[1]);
		//std::cout << " onchange(" << (unsigned)node->byte << ")! my children's hashes are " << getHash(node->child[0]) << "," << getHash(node->child[1]) << ". Mine is " << node->hash << std::endl;
		if (node->parent != NULL)
			onchange(node->parent);
	}

	static void onParentChange(void* elem)
	{
		merkle_node<HashType>* parent;

		node_ptr node(elem);
		if (node.isLeaf())
			parent = node.leaf()->parent;
		else
			parent = node.node()->parent;

		if (parent != NULL)
			onchange(parent);
	}

protected:
	static HashType getHash(void* elem)
	{
		node_ptr node(elem);
		if (node.isLeaf())
			return node.leaf()->second;
		else
			return node.node()->hash;
	}
};

template <typename KeyType>
struct merkle_location
{
	KeyType key;
	unsigned keybits;

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
class merkle_tree : public critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >
{
protected:
	using pair = merkle_pair<KeyType,HashType>;
	using tree_type = critbit_tree< pair, const pair&, merkle_node<HashType> >;

	using critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >::_tree;

public:
	// want top level state

	// returns whether true if we found a node, false if we found a leaf
	bool hash_lookup(const merkle_location<KeyType>& location, HashType& hash)
	{
		unsigned keylen = location.keybits/8;
		unsigned char bitmask = location.keybits%8;
		if (bitmask > 0)
			++keylen;
		bitmask = (1 << (8-bitmask)) - 1;

		typename tree_type::node_ptr node_ptr = _tree.subtree(pair(location.key), bitmask, keylen); // what about prefix, size?
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

	// match key,keybits,hash against the tree, returning two children if match fails
	bool diff() const
	{

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
