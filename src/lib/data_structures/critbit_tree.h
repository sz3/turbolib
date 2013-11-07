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

// internal storage = ValType* = char* | FooType*
// external comparisons = ExternalType = const char* | const FooType& (with operator to cast)
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

	ValType* find(ExternalType val) const
	{
		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_helper<ValType>::key_size(keybytes);
		if (empty())
			return NULL;

		return walkTreeForBestMember(_root, keybytes, keylen);
	}

	bool contains(ExternalType val) const
	{
		ValType* p = find(val);
		if (p == NULL)
			return false;

		// well this isn't hideous at all
		return 0 == strcmp((const char*)(const uint8_t*)val, critbit_helper<ValType>::c_str(p));
	}

	// 0 == oom
	// 1 == u already exists
	// 2 == added u
	int insert(ExternalType val)
	{
		const size_t ulen = critbit_helper<ValType>::size(val);
		if (empty())
			return insertIntoEmptyTree(val, ulen);

		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_helper<ValType>::key_size(keybytes);

		ValType* bestMember = walkTreeForBestMember(_root, keybytes, keylen);
		const uint8_t* p = (const uint8_t*)critbit_helper<ValType>::downcast(bestMember);
		const size_t plen = critbit_helper<ValType>::key_size(p);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, plen, keybytes, keylen, newbyte, newotherbits, newdirection))
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
		critbit_helper<ValType>::construct(x, val, ulen);
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

	// 0 if no changes
	// 1 if we deleted the thing
	int remove(ExternalType val)
	{
		const uint8_t* keybytes = (const uint8_t*)val;
		const size_t keylen = critbit_helper<ValType>::key_size(keybytes);

		ValType* p = (ValType*)_root;
		if (p == NULL)
			return 0;

		void** wherep = &_root;
		void** whereq = 0;
		node* q = 0;
		int direction = 0;

		// walk tree for best match
		while (1 & (intptr_t)p)
		{
			whereq = wherep;
			q = (node*)((intptr_t)p-1);
			direction = calculateDirection(q, keybytes, keylen);
			wherep = q->child + direction;
			p = (ValType*)*wherep;
		}

		// check best match
		if (0 != strcmp((const char*)keybytes, critbit_helper<ValType>::c_str(p)))
			return 0;
		free(p);

		// remove the node
		if (whereq == NULL)
			_root = 0;
		else
		{
			*whereq = q->child[1-direction];
			free(q);
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

	int insertIntoEmptyTree(ExternalType val, size_t bytes)
	{
		ValType* x;
		int a = posix_memalign((void**)&x, sizeof(void*), bytes);
		if (a)
			return 0;
		critbit_helper<ValType>::construct(x, val, bytes);
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
class critbit_helper
{
public:
	static size_t size(const ValType& val)
	{
		return sizeof(val);
	}

	static size_t key_size(const uint8_t* key)
	{
		return ValType::key_size(key);
	}

	static const char* c_str(ValType* val)
	{
		return (const char*)(const uint8_t*)downcast(val);
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
class critbit_helper<char>
{
public:
	static size_t size(const char* val)
	{
		return strlen(val)+1;
	}

	static size_t key_size(const uint8_t* key)
	{
		return strlen((const char*)key);
	}

	static const char* c_str(char* val)
	{
		return (const char*)val;
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
