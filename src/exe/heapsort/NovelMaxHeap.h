#pragma once

#include <iostream>

template <class SortableType>
class NovelMaxHeap
{
public:
	NovelMaxHeap(SortableType* buffer, unsigned size)
		: _buffer(buffer)
		, _size(size)
	{
		buildHeap();
	}

	void buildHeap()
	{
		unsigned i = _size>>1;
		while (i != 0)
		{
			--i;
			heapify(i);
			std::cout << "did buildHeap on " << i << std::endl;
			debugPrint();
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
		// else...
		++_buffer;
		--_size;

		heapify(0);
		return true;
	}

	void heapify(unsigned i)
	{
		unsigned index = i;
		while ( heapifyOnce(index) );
		debugPrint();
	}

	bool heapifyOnce(unsigned& index)
	{
		std::cout << "heapifyOnce on " << index << ":" << _buffer[index] << std::endl;
		// bounds check?
		unsigned left = (index<<1)+1;
		unsigned right = left+1;
		unsigned largest = index;

		if (left < _size && _buffer[left] > _buffer[largest])
			largest = left;
		if (right < _size && _buffer[right] > _buffer[largest])
			largest = right;

		std::cout << "heapifyOnce on " << index << ":" << _buffer[index]
				  << ". children: " << left << ":" << _buffer[left] << " and "
				  << right << ":" << _buffer[right] << std::endl;

		if (largest == index)
			return false;
		// else
		swap(index, largest);
		index = largest;
		return true;
	}

	void swap(unsigned index, unsigned other)
	{
		SortableType temp = _buffer[index];
		_buffer[index] = _buffer[other];
		_buffer[other] = temp;
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
};

