/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <algorithm>
#include <deque>
#include <set>
#include <utility>

namespace turbo {
template <typename KeyType, unsigned _limit=10>
class bounded_version_vector
{
public:
	struct clock
	{
		KeyType key;
		unsigned count;

		bool operator==(const clock& other) const
		{
			return key == other.key;
		}

		bool operator<(const clock& other) const
		{
			return key < other.key;
		}
	};

	enum COMPARE {
		EQUAL        =0,
		LESS_THAN    =1,
		GREATER_THAN =2,
		CONFLICT     =3,
	};

public:
	bounded_version_vector() {}

	bounded_version_vector(const std::deque<clock>& clocks)
		: _clocks(clocks)
	{
	}

	void increment(const KeyType& key)
	{
		typename std::deque<clock>::iterator it = std::find( _clocks.begin(), _clocks.end(), clock{key} );
		if (it == _clocks.end())
			_clocks.push_front(clock{key, 1});
		else
		{
			_clocks.push_front(clock{key, it->count+1});
			_clocks.erase(it);
		}
		if (_clocks.size() > _limit)
			_clocks.pop_back();
	}

	bool operator<(const bounded_version_vector& other) const
	{
		return compare(other) == LESS_THAN;
	}

	COMPARE compare(const bounded_version_vector& other) const
	{
		if (_clocks.size() > other._clocks.size())
		{
			unsigned result = compare(other._clocks, _clocks);
			if (result == GREATER_THAN)
				result = LESS_THAN;
			else if (result == LESS_THAN)
				result = GREATER_THAN;
			return (COMPARE)(result | GREATER_THAN);
		}

		return (COMPARE)compare(_clocks, other._clocks);
	}

	void merge(const bounded_version_vector& other)
	{
		unsigned noMatch = true;
		unsigned prevIndex = 0; // track the insert location based on whether we find a common pair, etc...
		for (typename std::deque<clock>::const_iterator it = other._clocks.begin(); it != other._clocks.end(); ++it)
		{
			typename std::deque<clock>::iterator current = std::find(_clocks.begin(), _clocks.end(), *it);
			if (current == _clocks.end())
				_clocks.insert(_clocks.begin()+(prevIndex++), *it);
			else if (current->count < it->count)
			{
				_clocks.erase(current);
				_clocks.insert(_clocks.begin()+prevIndex++, *it);
			}
			else //if (current->count >= it->count)
			{
				unsigned currentIndex = (current - _clocks.begin());
				if (noMatch)
					noMatch = reorient(prevIndex, currentIndex); // reorient to old prevIndex

				if (prevIndex <= currentIndex)
					prevIndex = currentIndex+1;
			}
		}
		if (noMatch)
			reorient(other._clocks.size(), _clocks.size());

		while (_clocks.size() > _limit)
			_clocks.pop_back();
	}

	void clear()
	{
		_clocks.clear();
	}

	bool empty() const
	{
		return _clocks.empty();
	}

	std::deque<clock> clocks() const
	{
		return _clocks;
	}

protected:
	typename std::deque<clock>::const_iterator findElem(const std::deque<clock>& clocks, const clock& elem) const
	{
		typename std::deque<clock>::const_iterator it = std::find(clocks.begin(), clocks.end(), elem);
		if (it != clocks.end() && it->count <= elem.count)
			return it;
		return clocks.end();
	}

	typename std::deque<clock>::const_iterator findEnd(const std::deque<clock>& clocks, const std::deque<clock>& candidates) const
	{
		if (candidates.size() >= _limit)
		{
			typename std::deque<clock>::const_iterator next = candidates.end()-(_limit>>1);
			for (; next != candidates.end(); ++next)
			{
				typename std::deque<clock>::const_iterator it = findElem(clocks, *next);
				if (it != clocks.end())
					return it;
			}
		}
		return clocks.end();
	}

	unsigned compare(const std::deque<clock>& lesser, const std::deque<clock>& greater) const
	{
		unsigned result = EQUAL;

		typename std::deque<clock>::const_iterator lesserEnd = findEnd(lesser, greater);
		typename std::deque<clock>::const_iterator greaterEnd = findEnd(greater, lesser);

		std::set<clock> first;
		std::copy(lesser.begin(), lesserEnd, std::inserter(first, first.begin()));
		std::set<clock> second;
		std::copy(greater.begin(), greaterEnd, std::inserter(second, second.begin()));

		for (typename std::set<clock>::const_iterator it = first.begin(); it != first.end(); ++it)
		{
			typename std::set<clock>::iterator other = second.find(*it);
			if (other == second.end())
			{
				result |= GREATER_THAN;
				continue;
			}

			if (it->count < other->count)
				result |= LESS_THAN;
			else if (other->count < it->count)
				result |= GREATER_THAN;
			second.erase(other);
		}
		if (!second.empty())
			result |= LESS_THAN;
		return result;
	}

	// attempt to restore a fair(ish) clock order after merging a conflict.
	bool reorient(unsigned additions, unsigned total)
	{
		// if no shared terms, interlace
		//   additions list is currently at the front [0-additions),
		//   ours is at the back. [additions-total)
		unsigned len = std::min(additions, total-additions);
		for (unsigned i = 0; i < len; ++i)
		{
			typename std::deque<clock>::iterator it = _clocks.begin() + additions + i;
			clock movingOnUp = *it;
			_clocks.erase(it);
			_clocks.insert(_clocks.begin()+(i<<1), movingOnUp);
		}
		return false;
	}

protected:
	std::deque<clock> _clocks;
};
} // namespace turbo
