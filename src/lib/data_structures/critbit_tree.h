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

template <typename ValType>
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

	// update all prototypes to use const& and copy semantics where appropriate, as well!
	ValType* find(const ValType* u) const
	{
		const uint8_t* ubytes = (const uint8_t*)u;
		const size_t ulen = critbit_helper<ValType*>::size(u);
		if (empty())
			return NULL;

		return walkTreeForBestMember(_root, ubytes, ulen); //FIXME?
	}

	bool contains(const ValType* u) const
	{
		uint8_t* p = (uint8_t*)find(u);
		if (p == NULL)
			return false;
		return critbit_helper<ValType*>::equals(u, (ValType*)p);
	}

	// 0 == oom
	// 1 == u already exists
	// 2 == added u
	int insert(const ValType* u)
	{
		const uint8_t* ubytes = (const uint8_t*)u;
		const size_t ulen = critbit_helper<ValType*>::size(u);
		if (empty())
			return insertIntoEmptyTree(u, ulen+1);

		uint8_t* p = (uint8_t*)walkTreeForBestMember(_root, ubytes, ulen);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, ubytes, ulen, newbyte, newotherbits, newdirection))
			return 1;

		// allocate node
		node* newnode;
		if (posix_memalign((void**)&newnode, sizeof(void*), sizeof(node)))
			return 0;
		ValType* x;
		if (posix_memalign((void**)&x, sizeof(void*), ulen+1))
		{
			free(newnode);
			return 0;
		}
		memcpy(x, ubytes, ulen+1);
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
			if (q->byte < ulen)
				c = ubytes[q->byte];
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

private:
	ValType* walkTreeForBestMember(void* p, const uint8_t* ubytes, size_t ulen) const
	{
		while (1 & (intptr_t)p)
		{
			node* q = (node*)((intptr_t)p-1);
			p = q->child[calculateDirection(q, ubytes, ulen)];
		}
		return (ValType*)p;
	}

	int calculateDirection(node* q, const uint8_t* ubytes, size_t ulen) const
	{
		uint8_t c = 0;
		if (q->byte < ulen)
			c = ubytes[q->byte];
		return direction(q->otherbits, c);
	}

	int direction(uint8_t bits, uint8_t c) const
	{
		return (1 + (bits | c)) >> 8;
	}

	int insertIntoEmptyTree(const ValType* u, size_t bytes)
	{
		char* x;
		int a = posix_memalign((void**)&x, sizeof(void*), bytes);
		if (a)
			return 0;
		memcpy(x, u, bytes);
		_root = x;
		return 2;
	}

	bool findCriticalBit(uint8_t* p, const uint8_t* ubytes, size_t ulen, uint32_t& newbyte, uint32_t& newotherbits, int& newdirection) const
	{
		if (!findDifferingByte(p, ubytes, ulen, newbyte, newotherbits))
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

	bool findDifferingByte(uint8_t* p, const uint8_t* ubytes, size_t ulen, uint32_t& newbyte, uint32_t& newotherbits) const
	{
		for (newbyte = 0; newbyte < ulen; ++newbyte)
		{
			if (p[newbyte] != ubytes[newbyte])
			{
				newotherbits = p[newbyte] ^ ubytes[newbyte];
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

	static bool equals(const ValType& left, const ValType& right)
	{
		return left == right;
	}
};

// cstring
template <>
class critbit_helper<char*>
{
public:
	static size_t size(const char* val)
	{
		return strlen(val);
	}

	static bool equals(const char* left, const char* right)
	{
		return 0 == strcmp(left, right);
	}
};
