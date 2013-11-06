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

// prototype for length, comparison operations
template <typename ValType> class critbit_helper;

template <typename ValType, typename ExternalType=const ValType&>
class critbit_tree
{
protected:
	struct node
	{
		void* child[2];
		uint32_t byte;
		uint8_t otherbits;
	};

public:
	critbit_tree()
		: _root(NULL)
	{}

	~critbit_tree()
	{
		clear();
	}

	// internal storage = char* | FooType*
	// external comparisons = const char* | const FooType& (with operator to cast)
	//

	// update all prototypes to use const& and copy semantics where appropriate, as well!
	ValType* find(ExternalType val) const
	{
		const uint8_t* ubytes = (const uint8_t*)val;
		const size_t ulen = critbit_helper<ValType>::size(val);
		if (empty())
			return NULL;

		return walkTreeForBestMember(_root, ubytes, ulen);
	}

	bool contains(ExternalType val) const
	{
		ValType* p = find(val);
		if (p == NULL)
			return false;

		// well this isn't hideous at all
		return 0 == strcmp((const char*)(const uint8_t*)val, (const char*)(const uint8_t*)critbit_helper<ValType>::downcast(p));
	}

	// 0 == oom
	// 1 == u already exists
	// 2 == added u
	int insert(ExternalType val)
	{
		const size_t ulen = critbit_helper<ValType>::size(val);
		const ValType* uptr = critbit_helper<ValType>::upcast(val);
		if (empty())
			return insertIntoEmptyTree(uptr, ulen);

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = strlen((const char*)keybytes);
		ValType* bestMember = walkTreeForBestMember(_root, keybytes, keylen);
		const uint8_t* p = (const uint8_t*)critbit_helper<ValType>::downcast(bestMember);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, keybytes, keylen, newbyte, newotherbits, newdirection))
			return 1;

		// allocate node
		node* newnode;
		if (posix_memalign((void**)&newnode, sizeof(void*), sizeof(node)))
			return 0;
		ValType* x;
		if (posix_memalign((void**)&x, sizeof(void*), ulen))
		{
			free(newnode);
			return 0;
		}
		memcpy(x, uptr, ulen);
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
			node* q = (node*)(p-1);
			if (q->byte > newbyte)
				break;
			if (q->byte == newbyte && q->otherbits > newotherbits)
				break;
			uint8_t c = 0;
			if (q->byte < keylen)
				c = keybytes[q->byte];
			wherep = q->child + direction(q->otherbits, c);
		}
		newnode->child[newdirection] = *wherep;
		*wherep = (void*)(1 + (char*)newnode);

		return 2;
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
	void clear(void* top)
	{
		ValType* p = (ValType*)top;
		if (1 & (intptr_t)p)
		{
			node* q = (node*)((intptr_t)p-1);
			clear(q->child[0]);
			clear(q->child[1]);
			free(q);
		}
		else
			free(p);
	}

	ValType* walkTreeForBestMember(void* p, const uint8_t* keybytes, size_t keylen) const
	{
		while (1 & (intptr_t)p)
		{
			node* q = (node*)((intptr_t)p-1);
			p = q->child[calculateDirection(q, keybytes, keylen)];
		}
		return (ValType*)p;
	}

	int calculateDirection(node* q, const uint8_t* keybytes, size_t keylen) const
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

	int insertIntoEmptyTree(const ValType* u, size_t bytes)
	{
		ValType* x;
		int a = posix_memalign((void**)&x, sizeof(void*), bytes);
		if (a)
			return 0;
		memcpy(x, u, bytes);
		_root = x;
		return 2;
	}

	bool findCriticalBit(const uint8_t* p, const uint8_t* keybytes, size_t keylen, uint32_t& newbyte, uint32_t& newotherbits, int& newdirection) const
	{
		if (!findDifferingByte(p, keybytes, keylen, newbyte, newotherbits))
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

	bool findDifferingByte(const uint8_t* p, const uint8_t* keybytes, size_t keylen, uint32_t& newbyte, uint32_t& newotherbits) const
	{
		for (newbyte = 0; newbyte < keylen; ++newbyte)
		{
			if (p[newbyte] != keybytes[newbyte])
			{
				newotherbits = p[newbyte] ^ keybytes[newbyte];
				return true;
			}
		}
		if (p[newbyte] != 0)
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
class critbit_helper
{
public:
	static size_t size(const ValType& val)
	{
		return sizeof(val);
	}

	static const ValType& downcast(ValType* val)
	{
		return *val;
	}

	static const ValType* upcast(const ValType& val)
	{
		return &val;
	}
};

// cstring
template <>
class critbit_helper<char>
{
public:
	static size_t size(const char* val)
	{
		return strlen(val)+1;
	}

	static const char* downcast(char* val)
	{
		return (const char*)val;
	}

	static const char* upcast(const char* val)
	{
		return val;
	}
};
