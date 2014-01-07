/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <algorithm>
#include <iostream>
#include <cstring>

template <class SortableType>
void doMerge(SortableType* left, SortableType* lend, SortableType* right, SortableType* rend, SortableType* workBuffer)
{
	while (1)
	{
		if (left != lend && (right == rend || *left < *right))
		{
			*workBuffer = *left;
			++left;
		}
		else if (right != rend)
		{
			*workBuffer = *right;
			++right;
		}
		else
			break;
		++workBuffer;
	}
}

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
void mergesort_index_based(SortableType* buffer, SortableType* workBuffer, unsigned size)
{
	::memcpy(workBuffer, buffer, sizeof(SortableType)*size);
	for (unsigned run = 1; run < size; run *= 2)
	{
		for (unsigned i = 0; i < size; i += 2*run)
			doMerge(buffer, i, std::min(i+run, size), std::min(i+2*run, size), workBuffer);
		::memcpy(buffer, workBuffer, sizeof(SortableType)*size);
	}
}

template <class SortableType>
SortableType* mergesort(SortableType* buffer, SortableType* workBuffer, unsigned size)
{
	for (unsigned run = 1; run < size; run *= 2)
	{
		for (unsigned i = 0; i < size; i += 2*run)
		{
			SortableType* left = buffer + i;
			SortableType* right = buffer + std::min(i+run, size);
			SortableType* end = buffer + std::min(i+2*run, size);
			doMerge(left, right, right, end, workBuffer+i);
		}

		SortableType* temp = buffer;
		buffer = workBuffer;
		workBuffer = temp;
	}
	return buffer;
}

template <class SortableType>
bool mergesort(SortableType* buffer, unsigned size)
{
	if (buffer == NULL)
		return false;

	SortableType* workBuffer = new (std::nothrow) SortableType[size];
	if (workBuffer == NULL)
		return false;

	SortableType* resbuffer = mergesort(buffer, workBuffer, size);
	if (resbuffer != buffer)
		::memcpy(buffer, workBuffer, sizeof(SortableType)*size);

	delete[] workBuffer;
	return true;
}
