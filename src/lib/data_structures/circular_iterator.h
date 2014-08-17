#pragma once

// should be const_circular_iterator?

template <typename Iterator>
class circular_iterator : public Iterator
{
public:
	circular_iterator(const Iterator& it, const Iterator& containerBegin, const Iterator& containerEnd)
		: Iterator(it)
		, _end(it)
		, _containerBegin(containerBegin)
		, _containerEnd(containerEnd)
	{
	}

	circular_iterator& operator++()
	{
		Iterator::operator++();
		if (*this == _containerEnd)
			Iterator::operator=(_containerBegin);
		if (*this == _end)
			Iterator::operator=(_containerEnd);
		return *this;
	}

protected:
	Iterator _end;
	Iterator _containerBegin;
	Iterator _containerEnd;
};
