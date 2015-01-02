/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <functional>

template<unsigned i>
class unpack_tuple
{
public:
	template<typename ReturnType, typename KeyType, typename ...TupleArgs, typename ...FunctArgs>
	inline static ReturnType unpack(const std::function<ReturnType(const KeyType&, const TupleArgs&...)>& fun, const KeyType& key, const std::tuple<TupleArgs...>& tuple, const FunctArgs&... funargs)
	{
		return unpack_tuple<i-1>::unpack(fun, key, tuple, std::get<i-1>(tuple), funargs...);
	}
};

template<>
class unpack_tuple<0>
{
public:
	template<typename ReturnType, typename KeyType, typename ...TupleArgs, typename ...FunctArgs>
	inline static ReturnType unpack(const std::function<ReturnType(const KeyType&, const TupleArgs&...)>& fun, const KeyType& key, const std::tuple<TupleArgs...>& tuple, const FunctArgs&... funargs)
	{
		return fun(key, funargs...);
	}
};
