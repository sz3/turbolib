#pragma once

#include <iostream>

template <class SortableType>
class NovelMaxHeap
{
public:
	NovelMaxHeap(SortableType* buffer, unsigned size)
		: _buffer(buffer)
		, _size(size)
		, _temp(0)
	{
		buildHeap();
	}

	void buildHeap()
	{
		unsigned i = _size >> 1;
		while (i != 0)
		{
			--i;
			heapify(i);
			//debugPrint();
		}
	}

	const SortableType* peek() const
	{
		return _buffer;
	}

	// doesn't need to return anything, since the memory for heapsort is shared...
	bool pop()
	{
		if (_size == 0)
			return false;

		swap(0, _size-1);
		--_size;

		if (_size > 1)
			heapify(0);
		return true;
	}

	void heapify(unsigned i)
	{
		unsigned index = i;
		while ( heapifyOnce(index) );
		//debugPrint();
	}

	bool heapifyOnce(unsigned& index)
	{
		// bounds check?
		unsigned left = (index<<1)+1;
		unsigned right = left+1;
		unsigned largest = index;

		if (left < _size && _buffer[left] > _buffer[largest])
			largest = left;
		if (right < _size && _buffer[right] > _buffer[largest])
			largest = right;

		if (largest == index)
			return false;
		// else
		swap(index, largest);
		index = largest;
		return true;
	}

	void swap(unsigned index, unsigned other)
	{
		_temp = _buffer[index];
		_buffer[index] = _buffer[other];
		_buffer[other] = _temp;
	}

	void debugPrint()
	{
		for (unsigned i = 0; i < _size; ++i)
		{
			if (i != 0)
				std::cout << ",";
			std::cout << _buffer[i];
		}
		std::cout << std::endl;
	}

protected:
	SortableType* _buffer;
	unsigned _size;
	SortableType _temp;
};

