#pragma once

#include <algorithm>
#include <iostream>
#include <cstring>

// better version, but not yet...
/*template <class SortableType>
void doMerge(SortableType* left, SortableType* right, SortableType* end, SortableType* workBuffer)
{
	SortableType* ll = left;
	SortableType* rr = right;
	for (
}*/

template <class SortableType>
void doMerge(SortableType* data, unsigned left, unsigned right, unsigned end, SortableType* workBuffer)
{
	unsigned ll = left;
	unsigned rr = right;
	for (unsigned i = ll; i < end; ++i)
	{
		if (ll < right && (rr >= end || data[ll] <= data[rr]))
		{
			workBuffer[i] = data[ll];
			++ll;
		}
		else if (rr < end)
		{
			workBuffer[i] = data[rr];
			++rr;
		}
		else
		{
			std::cout << "bad things? Breaking early." << std::endl;
			break;
		}
	}
}

template <class SortableType>
void mergesort(SortableType* buffer, SortableType* workBuffer, unsigned size)
{
	unsigned run = 1;
	for (unsigned run = 1; run < size; run *= 2)
	{
		for (unsigned i = 0; i < size; i += 2*run)
		{
			//SortableType* left = buffer + i;
			//SortableType* right = buffer + std::min(i+run, size);
			//SortableType* end = buffer + std::min(i+2*run, size);
			//doMerge(left, right, end, workBuffer);

			doMerge(buffer, i, std::min(i+run, size), std::min(i+2*run, size), workBuffer);
		}
		::memcpy(buffer, workBuffer, size);
	}
}

