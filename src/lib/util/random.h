/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <iterator>
#include <random>

namespace turbo {
namespace random
{
	template <typename Iter>
	Iter select(Iter begin, size_t size)
	{
		if (size <= 1)
			return begin;
		static std::random_device rd;
		static std::mt19937 gen(rd());

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
