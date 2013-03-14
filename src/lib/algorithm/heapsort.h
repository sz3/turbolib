#pragma once

#include "NovelMaxHeap.h"
#include <cstddef>
#include <iostream>

template <class SortableType>
void heapsort(SortableType* buffer, unsigned size)
{
	if (buffer == NULL)
		return;

	NovelMaxHeap<SortableType> heap(buffer, size);

	unsigned loop = 0;
	while (heap.pop())
	{
		//loop++;
		//std::cout << "loop " << loop << "!" << std::endl;
	}
}
