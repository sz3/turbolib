/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <iterator>
#include <random>

namespace turbo {
namespace random
{
	inline unsigned int seed()
	{
#ifdef DIET_RANDOM
		return ::rand();
#else
		static std::random_device rd;
		return rd();
#endif
	}

	inline std::string bytes(unsigned howmany)
	{
		std::string res;
		for (int i = 0; i < howmany; i += sizeof(unsigned int))
		{
			unsigned int entropy = seed();
			for (int b = 0; b < sizeof(unsigned int); ++b)
				res += *(reinterpret_cast<char*>(&entropy) + b);
		}
		return res;
	}

	template <typename Iter>
	Iter select(Iter begin, size_t size)
	{
		if (size <= 1)
			return begin;
		static std::mt19937 gen(seed());

		std::uniform_int_distribution<> dis(0, size-1);
		std::advance(begin, dis(gen));
		return begin;
	}

	template <typename Container>
	typename Container::const_iterator select(const Container& container)
	{
		return select(container.begin(), container.size());
	}

	template <typename Container>
	typename Container::iterator select(Container& container)
	{
		return select(container.begin(), container.size());
	}
}
} // namespace turbo
