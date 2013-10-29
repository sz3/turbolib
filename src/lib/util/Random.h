#pragma once

#include <iterator>
#include <random>

namespace Random
{
	template<typename Iter>
	Iter select(Iter start, Iter end, size_t size)
	{
		if (size <= 1)
			return start;
		static std::random_device rd;
		static std::mt19937 gen(rd());

		std::uniform_int_distribution<> dis(0, size-1);
		std::advance(start, dis(gen));
		return start;
	}
};

