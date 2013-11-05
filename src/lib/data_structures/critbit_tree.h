/*
adapted from DJB's critbit tree for NULL-terminated strings
	see: http://cr.yp.to/critbit.html
	see also: https://www.imperialviolet.org/2008/09/29/critbit-trees.html
*/

#pragma once

#include <cstdlib>
#include <cstdint>
#include <cstring>

// template the function compare well
template <typename ValType>//, typename Funct>
class critbit_tree
{
protected:
	class node
	{
	public:


	protected:
		void* child[2];
		uint32_t byte;
		uint8_t otherbits;
	};

public:
	// update all prototypes to use const& and copy semantics where appropriate, as well!
	bool contains(ValType* u) const
	{
		const uint8_t* ubytes = (void*)u;
		const size_t ulen = strlen(&u); // FIXME
		if (empty())
			return false;

		uint8_t* p = walkTreeForBestMember(_root, ubytes, ulen); //FIXME?
		return 0 == strcmp(u, (const char*)p); // FIXME
	}

	int insert(ValType* u)
	{
		const uint8_t* ubytes = (void*)u;
		const size_t ulen = strlen(u); // FIXME

		if (empty())
			return insertIntoEmptyTree(u, ulen+1);

		uint8_t* p = walkTreeForBestMember(_root, ubytes, ulen);

		uint32_t newbyte;
		uint32_t newotherbits;
		int newdirection;
		if (!findCriticalBit(p, ubytes, ulen, newbyte, newotherbits, newdirection))
			return 1;

		//13. inserting the new node
	}

	bool empty() const
	{
		return _root != NULL;
	}

protected:
	uint8_t* walkTreeForBestMember(uint8_t* p, const uint8_t* ubytes, size_t ulen) const
	{
		while (1 & (intptr_t)p)
		{
			node* q = (void*)(p-1);
			p = q->child[calculateDirection(q, ubytes, ulen)];
		}
		return p;
	}

	int calculateDirection(node* q, const uint8_t* ubytes, size_t ulen) const
	{
		uint8_t c = 0;
		if (q->byte < ulen)
			c = ubytes[q->byte];
		return direction(bits, c);
	}

	int direction(uint8_t bits, uint8_t c)
	{
		return (1 + (bits | c)) >> 8;
	}

	int insertIntoEmptyTree(ValType* u, size_t bytes)
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
		uint32_t newbyte;
		uint32_t newotherbits;
		if (!findDifferingByte(newbyte, newotherbits))
			return false;

		// find differing bit
		while (newotherbits & (newotherbits-1))
			newotherbits &= newotherbits-1;
		newotherbits ^= 255;

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

