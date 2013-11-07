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
		if (1 & (intptr_t)child)
		{
			merkle_node<HashType>* q = (merkle_node<HashType>*)((intptr_t)child-1);
			q->parent = parent;
		}
		else
		{
			LeafType* p = (LeafType*)child;
			assignParent(p, parent);
		}
	}

	static void inheritParent(void* successor, merkle_node<HashType>* child)
	{
		assignParent(successor, child->parent);
	}

	static void onchange(merkle_node<HashType>* node)
	{
		node->hash = getHash(node->child[0]) ^ getHash(node->child[1]);
	}

protected:
	static HashType getHash(void* child)
	{
		if (1 & (intptr_t)child)
		{
			merkle_node<HashType>* q = (merkle_node<HashType>*)((intptr_t)child-1);
			return q->hash;
		}
		else
		{
			LeafType* p = (LeafType*)child;
			return p->second;
		}
	}
};

template <typename KeyType, typename HashType>
using merkle_tree = critbit_map< KeyType, HashType, merkle_node<HashType>, merkle_pair<KeyType,HashType> >;
