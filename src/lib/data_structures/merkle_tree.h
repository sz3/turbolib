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

template <typename KeyType, typename HashType>
class merkle_tree : public critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >
{
public:

};
