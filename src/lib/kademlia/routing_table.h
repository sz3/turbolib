/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <algorithm>
#include <deque>
#include <random>

namespace kademlia {

template <typename ValueType, template <typename...> class ListType, typename Hash=std::hash<ValueType>>
class routing_table
{
protected:
	using HashType = typename Hash::result_type;
	using list = ListType<ValueType>;
	using table = std::array<list, sizeof(HashType)*8>;

	template <typename ListIterator, typename TableIterator>
	class _iterator : public ListIterator
	{
	public:
		_iterator(const ListIterator& it, const TableIterator& begin, const TableIterator& end)
			: ListIterator(it)
			, _current(begin)
			, _end(end)
		{
			if (begin != end)
				iterate_until_valid_or_end();
		}

		_iterator& operator++()
		{
			ListIterator::operator++();
			iterate_until_valid_or_end();
			return *this;
		}

	protected:
		void iterate_until_valid_or_end()
		{
			while (*this == _current->end())
			{
				++_current;
				if (_current == _end)
					break;
				ListIterator::operator=(_current->begin());
			}
		}

	protected:
		TableIterator _current;
		TableIterator _end;
	};

public:
	using list_iterator = typename list::const_iterator;
	using iterator = _iterator<typename list::iterator, typename table::iterator>;
	using const_iterator = _iterator<typename list::const_iterator, typename table::const_iterator>;

public:
	void set_origin(const ValueType& value)
	{
		_origin = hash(value);
	}

	bool insert(const ValueType& value)
	{
		unsigned i = index(value);
		list& bucket = _table[i];
		if (std::find(bucket.begin(), bucket.end(), value) != bucket.end())
			return false;

		bucket.push_back(value);
		++_size;
		return true;
	}

	template <typename LookupType>
	bool erase(const LookupType& key)
	{
		unsigned i = index(key);
		list& bucket = _table[i];

		auto it = std::find(bucket.begin(), bucket.end(), key);
		if (it == bucket.end())
			return false;

		bucket.erase(it);
		--_size;
		return true;
	}

	template <typename LookupType>
	const_iterator find(const LookupType& key) const
	{
		unsigned i = index(key);
		const list& bucket = _table[i];

		auto it = std::find(bucket.begin(), bucket.end(), key);
		if (it == bucket.end())
			return const_iterator(end(), _table.end(), _table.end());
		return const_iterator(it, _table.begin() + i, _table.end());
	}

	// doesn't reorient the data structure.
	// i.e, only use this if your aren't changing the hashable part of the key.
	template <typename LookupType>
	iterator find(const LookupType& key)
	{
		unsigned i = index(key);
		list& bucket = _table[i];

		auto it = std::find(bucket.begin(), bucket.end(), key);
		if (it == bucket.end())
			return iterator(_table.back().end(), _table.end(), _table.end());
		return iterator(it, _table.begin() + i, _table.end());
	}

	const_iterator random() const
	{
		std::random_device rd;
		std::mt19937 rando(rd());

		if (_size == 0)
			return begin();

		else if (_size > 100)
		{
			// use the table structure to favor values closer to _origin
			std::uniform_int_distribution<unsigned int> table_range(0, _table.size()-1);
			typename table::const_iterator bucket = _table.begin() + table_range(rando);
			if ( !bucket->empty() )
			{
				std::uniform_int_distribution<unsigned int> bucket_range(0, bucket->size()-1);
				return const_iterator(bucket->begin() + bucket_range(rando), bucket, _table.end());
			}

			const_iterator it = const_iterator(bucket->begin(), bucket, _table.end());
			if (it != end())
				return it;
		}

		// if nodes <= 100, or if the clever lookup failed, iterate to a random value
		const_iterator it = begin();
		std::uniform_int_distribution<unsigned int> full_range(0, _size-1);
		unsigned adv = full_range(rando);
		for (unsigned i = 0; i < adv; ++i)
			++it;
		return it;
	}

	const_iterator begin() const
	{
		return const_iterator(_table.begin()->begin(), _table.begin(), _table.end());
	}

	list_iterator end() const
	{
		return _table.back().end();
	}

	unsigned size() const
	{
		return _size;
	}

protected:
	template <typename LookupType>
	unsigned index(const LookupType& key) const
	{
		HashType mask = hash(key) ^ _origin;
		unsigned index = 0;
		while (mask >>= 1)
			++index;
		return index;
	}

protected:
	Hash hash;
	table _table;
	HashType _origin;
	unsigned _size = 0;
};

}//namespace
