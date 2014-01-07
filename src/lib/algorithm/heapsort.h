/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include "NovelMaxHeap.h"
#include <cmath>
#include <cstddef>
#include <iostream>

template <class SortableType>
void heapsort(SortableType* buffer, unsigned size)
{
	if (buffer == NULL)
		return;

	NovelMaxHeap<SortableType> heap(buffer, size);
	while (heap.pop());

#ifdef SOOPER_DEBUG
	std::cout << "log2(" << size << ") = " << log2(size) << ", build heap: " << (heap._buildHeapCount*1.0/size) << ", heapify: " << (heap._heapifyCount*1.0/size) << std::endl;
#endif
}
