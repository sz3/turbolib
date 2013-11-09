/*
adapted from DJB's critbit tree for NULL-terminated strings
	see: http://cr.yp.to/critbit.html
	see also: https://www.imperialviolet.org/2008/09/29/critbit-trees.html
*/

#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>

// base critbit_node class.
// also possible to inherit from this, add stuff, and pass in as template argument.
struct critbit_node
{
	void* child[2];
	uint32_t byte;
	uint8_t otherbits;
};

// utility class to browse the tree
template <typename ValType, typename Node>
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
		return !isNode();
	}

	bool isNode() const
	{
		return (1 & (intptr_t)_ptr);
	}

	Node* node() const
	{
		return (Node*)((intptr_t)_ptr-1);
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
template <typename ValType, typename Node> class critbit_ext;

// internal storage = ValType* = char* | FooType*
// external comparisons = ExternalType = const char* | const FooType& (with operator to cast)
template <typename ValType, typename ExternalType=const ValType&, typename Node=critbit_node>
class critbit_tree
{
public:
	using node_ptr = critbit_node_ptr<ValType,Node>;

public:
	critbit_tree()
		: _root(NULL)
	{}

	~critbit_tree()
	{
		clear();
	}

	// TODO: prefix is different from the normal value lookup -- it might be intentionally shorter.
	node_ptr subtree(ExternalType prefix, unsigned char bitmask = 0, size_t keylen = 0) const
	{
		if (empty())
			return NULL;

		const uint8_t* keybytes = (const uint8_t*)prefix;
		if (keylen == 0)
			keylen = critbit_elem_ops<ValType>::key_size(prefix);

		node_ptr p;
		node_ptr top = p = _root;
		while (p.isNode())
		{
			Node* q = p.node();
			if (q->byte+1 >= keylen)
			{
				if (q->byte >= keylen || ((q->otherbits ^ 0xFF) & bitmask) > 0)
					break;
			}
			p = q->child[calculateDirection(q, keybytes, keylen)];
			top = p;
		}
		return top;
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
		ValType* p = lower_bound(val);
		if (p == NULL)
			return false;

		return critbit_elem_ops<ValType>::equals(val, critbit_elem_ops<ValType>::downcast(p));
	}

	// 0 == oom
	// 1 == u already exists
	// 2 == added u
	int insert(ExternalType val)
	{
		const size_t ulen = critbit_elem_ops<ValType>::size(val);
		if (empty())
			return insertIntoEmptyTree(val, ulen);

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_elem_ops<ValType>::key_size(val);

		ExternalType bestMember = critbit_elem_ops<ValType>::downcast(walkTreeForBestMember(_root, keybytes, keylen));
		const uint8_t* p = (const uint8_t*)bestMember;
		const size_t plen = critbit_elem_ops<ValType>::key_size(bestMember);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, plen, keybytes, keylen, newbyte, newotherbits, newdirection))
			return 1;
		//critbit_ext<Node>::onchange(bestMember);

		// allocate node
		Node* newnode;
		if (posix_memalign((void**)&newnode, sizeof(void*), sizeof(Node)))
			return 0;
		ValType* x;
		if (posix_memalign((void**)&x, sizeof(void*), ulen))
		{
			free(newnode);
			return 0;
		}
		critbit_elem_ops<ValType>::construct(x, val, ulen);
		newnode->byte = newbyte;
		newnode->otherbits = newotherbits;
		newnode->child[1-newdirection] = x;

		// insert node into tree
		void** wherep = &_root;
		for (;;)
		{
			if (!(1 & (intptr_t)*wherep))
				break;
			uint8_t* p = (uint8_t*)*wherep;
			Node* q = (Node*)(p-1);
			if (q->byte > newbyte)
				break;
			if (q->byte == newbyte && q->otherbits > newotherbits)
				break;
			wherep = q->child + calculateDirection(q, keybytes, keylen);
		}
		newnode->child[newdirection] = *wherep;
		*wherep = (void*)(1 + (char*)newnode);

		// instead of messing around with parents, what about a "modified list", that gets [optionally] auto-populated on insert/delete?
		// deque or something, preserves the inheritance order and just calls onchange on each guy.
		critbit_ext<ValType,Node>::inheritParent(newnode, newnode->child[newdirection]);
		critbit_ext<ValType,Node>::assignParent(x, newnode);
		critbit_ext<ValType,Node>::assignParent(newnode->child[newdirection], newnode);
		critbit_ext<ValType,Node>::onchange(newnode);

		return 2;
	}

	// 0 if no changes
	// 1 if we deleted the thing
	int remove(ExternalType val)
	{
		if (empty())
			return 0;

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_elem_ops<ValType>::key_size(val);

		ValType* p = (ValType*)_root;
		void** wherep = &_root;
		Node* q = 0;
		void** whereq = 0;
		int direction = 0;

		// walk tree for best match
		while (1 & (intptr_t)p)
		{
			whereq = wherep;
			q = (Node*)((intptr_t)p-1);
			direction = calculateDirection(q, keybytes, keylen);
			wherep = q->child + direction;
			p = (ValType*)*wherep;
		}

		// check best match
		if (!critbit_elem_ops<ValType>::equals(val, critbit_elem_ops<ValType>::downcast(p)))
			return 0;
		free(p);

		// remove the node
		if (whereq == NULL)
			_root = 0;
		else
		{
			*whereq = q->child[1-direction];
			critbit_ext<ValType,Node>::inheritParent(*whereq, q);
			free(q);
			critbit_ext<ValType,Node>::onParentChange(*whereq);
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
		if (p.isNode())
		{
			Node* q = p.node();
			clear(q->child[0]);
			clear(q->child[1]);
			free(q);
		}
		else
			free(p.leaf());
	}

	ValType* walkTreeForBestMember(node_ptr p, const uint8_t* keybytes, size_t keylen) const
	{
		while (p.isNode())
		{
			Node* q = p.node();
			p = q->child[calculateDirection(q, keybytes, keylen)];
		}
		return p.leaf();
	}

	int calculateDirection(Node* q, const uint8_t* keybytes, size_t keylen) const
	{
		uint8_t c = 0;
		if (q->byte < keylen)
			c = keybytes[q->byte];
		return direction(q->otherbits, c);
	}

	int direction(uint8_t bits, uint8_t c) const
	{
		return (1 + (bits | c)) >> 8;
	}

	int insertIntoEmptyTree(ExternalType val, size_t bytes)
	{
		ValType* x;
		int a = posix_memalign((void**)&x, sizeof(void*), bytes);
		if (a)
			return 0;
		critbit_elem_ops<ValType>::construct(x, val, bytes);
		critbit_ext<ValType,Node>::assignParent(x, NULL);
		_root = x;
		return 2;
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
template <typename ValType, typename Node>
class critbit_ext
{
public:
	static void assignParent(ValType* child, Node* parent)
	{
	}

	static void assignParent(void* child, Node* parent)
	{
	}

	static void inheritParent(void* successor, Node* child)
	{
	}

	static void inheritParent(Node* successor, void* child)
	{
	}

	static void onchange(Node* node)
	{
	}

	static void onParentChange(void* node)
	{
	}
};
