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

template <typename LeafType, typename HashType>
class critbit_ext< LeafType, merkle_node<HashType> >
{
public:
	static void assignParent(LeafType* child, merkle_node<HashType>* parent)
	{
		child->parent = parent;
	}

	static void assignParent(void* child, merkle_node<HashType>* parent)
	{
		merkle_node<HashType>* node;
		LeafType* leaf;
		if (isLeaf(child, node, leaf))
			assignParent(leaf, parent);
		else
			node->parent = parent;
	}

	static void inheritParent(void* successor, merkle_node<HashType>* child)
	{
		assignParent(successor, child->parent);
	}

	static void inheritParent(merkle_node<HashType>* successor, void* child)
	{
		merkle_node<HashType>* node;
		LeafType* leaf;
		if (isLeaf(child, node, leaf))
			successor->parent = leaf->parent;
		else
			successor->parent = node->parent;
	}

	static void onchange(merkle_node<HashType>* node)
	{
		node->hash = getHash(node->child[0]) ^ getHash(node->child[1]);
		if (node->parent != NULL)
			onchange(node->parent);
	}

	static void onParentChange(void* elem)
	{
		merkle_node<HashType>* node;
		LeafType* leaf;
		if (isLeaf(elem, node, leaf))
			node = leaf->parent;
		else
			node = node->parent;

		if (node != NULL)
			onchange(node);
	}

protected:
	static HashType getHash(void* elem)
	{
		merkle_node<HashType>* node;
		LeafType* leaf;
		if (isLeaf(elem, node, leaf))
			return leaf->second;
		else
			return node->hash;
	}

	static bool isLeaf(void* elem, merkle_node<HashType>*& node, LeafType*& leaf)
	{
		if (1 & (intptr_t)elem)
		{
			node = (merkle_node<HashType>*)((intptr_t)elem-1);
			return false;
		}
		else
		{
			leaf = (LeafType*)elem;
			return true;
		}
	}
};

template <typename KeyType, typename HashType>
class merkle_tree : public critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >
{
public:

};
