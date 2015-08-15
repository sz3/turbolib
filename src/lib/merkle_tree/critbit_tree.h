/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
/*
adapted from DJB's critbit tree for NULL-terminated strings
	see: http://cr.yp.to/critbit.html
	see also: https://www.imperialviolet.org/2008/09/29/critbit-trees.html
*/

#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include "util/cstrfun.h" // for "strnncmp()"

namespace turbo {

struct critbit_branch
{
	void* child[2];
	uint32_t byte;
	uint8_t otherbits;
};

// utility class to browse the tree
template <typename ValType, typename Branch>
class critbit_node_ptr
{
public:
	critbit_node_ptr()
	{
	}

	critbit_node_ptr(void* ptr)
		: _ptr(ptr)
	{
	}

	bool isNull() const
	{
		return _ptr == NULL;
	}

	bool isLeaf() const
	{
		return !isBranch();
	}

	bool isBranch() const
	{
		return (1 & (intptr_t)_ptr);
	}

	Branch& branch() const
	{
		return *(Branch*)((intptr_t)_ptr-1);
	}

	ValType* leaf() const
	{
		return (ValType*)_ptr;
	}

protected:
	void* _ptr;
};

// prototype for length, comparison operations
template <typename ValType> class critbit_elem_ops;

// prototype for extensions to critbit operations.
// to remove, just nuke the template declaration at the bottom and any lines that include this prefix!
template <typename ValType, typename Branch> class critbit_ext;

// internal storage = ValType* = char* | FooType*
// external comparisons = ExternalType = const char* | const FooType& (with operator to cast)
template <typename ValType, typename ExternalType=const ValType&, typename Branch=critbit_branch>
class critbit_tree
{
public:
	using node_ptr = critbit_node_ptr<ValType,Branch>;

public:
	critbit_tree()
		: _root(NULL)
	{}

	~critbit_tree()
	{
		clear();
	}

	// nearest_subtree. No sanity check that our prefix actually matches what we found.
	node_ptr nearest_subtree(ExternalType prefix, unsigned char bitmask = 0, size_t keylen = ~0) const
	{
		if (empty())
			return NULL;

		const uint8_t* keybytes = (const uint8_t*)prefix;
		if (keylen == ~0)
			keylen = critbit_elem_ops<ValType>::key_size(prefix);

		// walk tree until we reach the end of the prefix
		// the bitmask is used to discard low-end disagreements in the last byte
		node_ptr top = _root;
		while (top.isBranch())
		{
			Branch& q = top.branch();
			if (q.byte+1 >= keylen)
			{
				if (q.byte >= keylen || ((q.otherbits ^ 0xFF) & bitmask) > 0)
					break;
			}
			top = q.child[calculateDirection(q, keybytes, keylen)];
		}
		return top;
	}

	node_ptr subtree(ExternalType prefix, unsigned char bitmask = 0, size_t keylen = ~0) const
	{
		node_ptr top = nearest_subtree(prefix, bitmask, keylen);
		if (top.isNull())
			return top;

		const uint8_t* keybytes = (const uint8_t*)prefix;
		if (keylen == ~0)
			keylen = critbit_elem_ops<ValType>::key_size(prefix);

		// test that we have a valid node
		if (keylen > 0)
		{
			ValType* leaf = top.isLeaf()? top.leaf() : begin(top);
			const uint8_t* leafbytes = (const uint8_t*)critbit_elem_ops<ValType>::downcast(leaf);

			for (unsigned i = 0; i < keylen-1; ++i)
				if (keybytes[i] != leafbytes[i])
					return NULL;
			if (((keybytes[keylen-1] xor leafbytes[keylen-1]) & ~bitmask) != 0)
				return NULL;
		}
		return top;
	}

	void enumerate(std::function<bool(ExternalType)> fun, ExternalType first, ExternalType last) const
	{
		const uint8_t* firstkey = (const uint8_t*)first;
		const size_t firstlen = critbit_elem_ops<ValType>::key_size(first);
		const uint8_t* lastkey = (const uint8_t*)last;
		const size_t lastlen = critbit_elem_ops<ValType>::key_size(last);

		node_ptr top;

		uint32_t diffbyte;
		uint32_t diffbits;
		int direction;
		if (!findCriticalBit(firstkey, firstlen, lastkey, lastlen, diffbyte, diffbits, direction))
			top = subtree(first);
		else
		{
			diffbits ^= 0xFF;
			diffbits |= diffbits>>1;
			diffbits |= diffbits>>2;
			diffbits |= diffbits>>4;
			top = subtree(first, diffbits, diffbyte+1);
		}
		if (top.isNull())
			return;

		// walk tree to first leaf in set.
		ValType* start = walkTreeForBestMember(top, firstkey, firstlen);

		// "start" may not be the right starting point. Specifically, we're worried that start < first
		ExternalType bestStart = critbit_elem_ops<ValType>::downcast(start);
		const uint8_t* startKey = (const uint8_t*)bestStart;
		const size_t startLen = critbit_elem_ops<ValType>::key_size(bestStart);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (findCriticalBit(startKey, startLen, firstkey, firstlen, newbyte, newotherbits, newdirection))
		{
			// direction is inverted, but basically what we're watching out for is that first is a "right branch" that doesn't exist in the tree.
			// that is, we need to go up one from our hypothetical branch's parent...
			if (newdirection == 0)
			{
				node_ptr node(top);
				node_ptr parent(top);
				while (node.isBranch())
				{
					Branch& q = node.branch();
					if (q.byte > newbyte)
						break;
					if (q.byte == newbyte && q.otherbits > newotherbits)
						break;
					parent = node;
					int dir = calculateDirection(q, firstkey, firstlen);
					node = q.child[dir];
				}
				// ...and set start to the parent's right branch's leftmost child. (a mouthful...)
				if (!parent.isBranch())
					return;
				start = begin(parent.branch().child[1]);
			}
		}

		// walk tree to find last leaf in set.
		ValType* stop = walkTreeForBestMember(top, lastkey, lastlen);

		// finding the appropriate stop location is like start, but reversed.
		ExternalType bestStop = critbit_elem_ops<ValType>::downcast(stop);
		const uint8_t* stopKey = (const uint8_t*)bestStop;
		size_t stopLen = critbit_elem_ops<ValType>::key_size(bestStop);

		if (findCriticalBit(stopKey, stopLen, lastkey, lastlen, newbyte, newotherbits, newdirection))
		{
			// direction is inverted, but basically what we're watching out for is that last is a "left branch" that doesn't exist in the tree.
			// that is, we need to go up one from our hypothetical branch's parent...
			if (newdirection == 1)
			{
				node_ptr node(top);
				node_ptr parent(top);
				while (node.isBranch())
				{
					Branch& branch = node.branch();
					if (branch.byte > newbyte)
						break;
					if (branch.byte == newbyte && branch.otherbits > newotherbits)
						break;
					parent = node;
					int dir = calculateDirection(branch, lastkey, lastlen);
					node = branch.child[dir];
				}
				// ...and set stop to the parent's left branch's rightmost child. (again, a mouthful...)
				if (parent.isBranch())
					stop = end(parent.branch().child[0]);
			}
		}

		ExternalType finalStop = critbit_elem_ops<ValType>::downcast(stop);
		stopKey = (const uint8_t*)finalStop;
		stopLen = critbit_elem_ops<ValType>::key_size(finalStop);

		bool excludeStop = (strnncmp((const char*)stopKey, stopLen, (const char*)lastkey, lastlen) > 0);
		enumerate(fun, top, start, stop, excludeStop);
	}

	int enumerate(std::function<bool(ExternalType)> fun, node_ptr top, ValType* start, ValType* stop, bool excludeStop, unsigned state=1) const
	{
		enum {
			STOP = 0,
			SEARCHING = 1,
			FOUND = 2,
		};
		if (top.isLeaf())
		{
			switch (state)
			{
				case SEARCHING:
					if (top.leaf() != start)
						break;
					state = FOUND;

				case FOUND:
				{
					if (excludeStop && top.leaf() == stop)
						return STOP;
					if (!fun(critbit_elem_ops<ValType>::downcast(top.leaf())))
						return STOP;
					if (top.leaf() == stop)
						return STOP;
					break;
				}
				default:
					return STOP;
			}
			return state;
		}
		else
		{
			Branch& branch = top.branch();
			if (state = enumerate(fun, branch.child[0], start, stop, excludeStop, state))
				return enumerate(fun, branch.child[1], start, stop, excludeStop, state);
			else
				return STOP;
		}
	}

	bool enumerate(std::function<bool(ExternalType)> fun, node_ptr top) const
	{
		if (top.isLeaf())
			return fun(critbit_elem_ops<ValType>::downcast(top.leaf()));
		else
		{
			Branch& branch = top.branch();
			if (enumerate(fun, branch.child[0]))
				return enumerate(fun, branch.child[1]);
			else
				return false;
		}
	}

	ValType* begin() const
	{
		if (empty())
			return NULL;
		return begin(_root);
	}

	ValType* begin(node_ptr root) const
	{
		while (root.isBranch())
			root = root.branch().child[0];
		return root.leaf();
	}

	ValType* end() const
	{
		if (empty())
			return NULL;
		return end(_root);
	}

	ValType* end(node_ptr root) const
	{
		while (root.isBranch())
			root = root.branch().child[1];
		return root.leaf();
	}

	// iterator would be wonderful
	ValType* lower_bound(ExternalType val) const
	{
		if (empty())
			return NULL;

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_elem_ops<ValType>::key_size(val);
		return walkTreeForBestMember(_root, keybytes, keylen);
	}

	bool contains(ExternalType val) const
	{
		return find(val) != NULL;
	}

	ValType* find(ExternalType val) const
	{
		ValType* p = lower_bound(val);
		if (p == NULL or !critbit_elem_ops<ValType>::equals(val, critbit_elem_ops<ValType>::downcast(p)) )
			return NULL;
		return p;
	}

	ValType* insert(ExternalType val)
	{
		bool is_new;
		return insert(val, is_new);
	}

	// return NULL == oom
	// is_new == if false, we didn't do anything (the element exists)
	ValType* insert(ExternalType val, bool& is_new)
	{
		const size_t ulen = critbit_elem_ops<ValType>::size(val);
		if (empty())
		{
			is_new = true;
			return insertIntoEmptyTree(val, ulen);
		}

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_elem_ops<ValType>::key_size(val);

		ValType* closest = walkTreeForBestMember(_root, keybytes, keylen);
		ExternalType extClosest = critbit_elem_ops<ValType>::downcast(closest);
		const uint8_t* p = (const uint8_t*)extClosest;
		const size_t plen = critbit_elem_ops<ValType>::key_size(extClosest);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, plen, keybytes, keylen, newbyte, newotherbits, newdirection))
		{
			is_new = false;
			return closest;
		}
		// TODO: _ext.onchange() for in-place updates?

		// allocate node
		Branch* newnode;
		if (posix_memalign((void**)&newnode, sizeof(void*), sizeof(Branch)) != 0)
			return NULL;
		ValType* x;
		if (posix_memalign((void**)&x, sizeof(void*), ulen) != 0)
		{
			free(newnode);
			return NULL;
		}
		critbit_elem_ops<ValType>::construct(x, val, ulen);
		newnode->byte = newbyte;
		newnode->otherbits = newotherbits;
		newnode->child[1-newdirection] = x;

		// insert node into tree
		void** where_at = &_root;
		for (;;)
		{
			node_ptr node(*where_at);
			if ( !node.isBranch() )
				break;
			Branch& branch = node.branch();
			if (branch.byte > newbyte)
				break;
			if (branch.byte == newbyte && branch.otherbits > newotherbits)
				break;
			where_at = branch.child + calculateDirection(branch, keybytes, keylen);
			_ext.push_change(&branch);
		}
		newnode->child[newdirection] = *where_at;
		*where_at = (void*)(1 + (char*)newnode);

		_ext.push_change(newnode);
		_ext.onchange();

		is_new = true;
		return x;
	}

	// 0 if no changes
	// 1 if we deleted the thing
	int remove(ExternalType val)
	{
		if (empty())
			return 0;

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_elem_ops<ValType>::key_size(val);

		node_ptr best(_root);
		void** where_best = &_root;
		Branch* parent = 0;
		void** where_parent = 0;
		int direction = 0;

		// walk tree for best match
		while (best.isBranch())
		{
			where_parent = where_best;
			parent = &best.branch();
			direction = calculateDirection(*parent, keybytes, keylen);
			where_best = parent->child + direction;
			best = *where_best;

			if (best.isBranch())
				_ext.push_change(parent);
		}

		// check best match
		if (!critbit_elem_ops<ValType>::equals(val, critbit_elem_ops<ValType>::downcast(best.leaf())))
		{
			_ext.clear_changes();
			return 0;
		}
		free(best.leaf());

		// remove the node
		if (where_parent == NULL)
			_root = 0;
		else
		{
			*where_parent = parent->child[1-direction];
			free(parent);
			_ext.onchange();
		}
		return 1;
	}

	bool empty() const
	{
		return _root == NULL;
	}

	void clear()
	{
		if (_root == NULL)
			return;
		clear(_root);
		_root = NULL;
	}

private:
	void clear(node_ptr p)
	{
		if (p.isBranch())
		{
			Branch& q = p.branch();
			clear(q.child[0]);
			clear(q.child[1]);
			Branch* tofree = &q;
			free(tofree);
		}
		else
			free(p.leaf());
	}

	ValType* walkTreeForBestMember(node_ptr p, const uint8_t* keybytes, size_t keylen) const
	{
		while (p.isBranch())
		{
			Branch& branch = p.branch();
			p = branch.child[calculateDirection(branch, keybytes, keylen)];
		}
		return p.leaf();
	}

	int calculateDirection(const Branch& branch, const uint8_t* keybytes, size_t keylen) const
	{
		uint8_t c = 0;
		if (branch.byte < keylen)
			c = keybytes[branch.byte];
		return direction(branch.otherbits, c);
	}

	int direction(uint8_t bits, uint8_t c) const
	{
		return (1 + (bits | c)) >> 8;
	}

	ValType* insertIntoEmptyTree(ExternalType val, size_t bytes)
	{
		ValType* x;
		if (posix_memalign((void**)&x, sizeof(void*), bytes) != 0)
			return NULL;
		critbit_elem_ops<ValType>::construct(x, val, bytes);
		_root = x;
		return x;
	}

	bool findCriticalBit(const uint8_t* p, size_t plen, const uint8_t* keybytes, size_t keylen, uint32_t& newbyte, uint32_t& newotherbits, int& newdirection) const
	{
		if (!findDifferingByte(p, plen, keybytes, keylen, newbyte, newotherbits))
			return false;

		// find differing bit
		newotherbits |= newotherbits>>1;
		newotherbits |= newotherbits>>2;
		newotherbits |= newotherbits>>4;
		newotherbits = (newotherbits & ~(newotherbits>>1)) ^ 0xFF;

		uint8_t c = p[newbyte];
		newdirection = direction(newotherbits, c);
		return true;
	}

	bool findDifferingByte(const uint8_t* p, size_t plen, const uint8_t* keybytes, size_t keylen, uint32_t& newbyte, uint32_t& newotherbits) const
	{
		for (newbyte = 0; newbyte < keylen; ++newbyte)
		{
			if (p[newbyte] != keybytes[newbyte])
			{
				newotherbits = p[newbyte] ^ keybytes[newbyte];
				return true;
			}
		}
		if (newbyte < plen)
		{
			newotherbits = p[newbyte];
			return true;
		}
		return false;
	}

protected:
	void* _root;
	critbit_ext<ValType, Branch> _ext;
};

template <typename ValType>
class critbit_elem_ops
{
public:
	static size_t size(const ValType& val)
	{
		return sizeof(val);
	}

	static size_t key_size(const ValType& val)
	{
		return val.key_size();
	}

	static bool equals(const ValType& left, const ValType& right)
	{
		return left == right;
	}

	static const ValType& downcast(ValType* val)
	{
		return *val;
	}

	// target has already allocated memory for us,
	// but we still need to put a new ValType into him
	static void construct(ValType* target, const ValType& source, size_t length)
	{
		new (target) ValType(source);
	}
};

// cstring
template <>
class critbit_elem_ops<char>
{
public:
	static size_t size(const char* val)
	{
		return strlen(val)+1;
	}

	static size_t key_size(const char* key)
	{
		return strlen(key);
	}

	static bool equals(const char* left, const char* right)
	{
		return 0 == strcmp(left, right);
	}

	static const char* downcast(char* val)
	{
		return (const char*)val;
	}

	static void construct(char* target, const char* source, size_t length)
	{
		memcpy(target, source, length);
	}
};

// empty functions to get optimised out. Specializations will implement special behavior.
template <typename ValType, typename Branch>
class critbit_ext
{
public:
	void push_change(Branch* branch)
	{
	}

	void clear_changes()
	{
	}

	void onchange()
	{
	}
};

} // namespace turbo
