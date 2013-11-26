#pragma once

#include <functional>

template<unsigned i>
class unpack_tuple
{
public:
	template<typename ReturnType, typename ...TupleArgs, typename ...FunctArgs>
	inline static ReturnType unpack(const std::function<ReturnType(const TupleArgs&...)>& fun, const std::tuple<TupleArgs...>& tuple, const FunctArgs&... funargs)
	{
		return unpack_tuple<i-1>::unpack(fun, tuple, std::get<i-1>(tuple), funargs...);
	}
};

template<>
class unpack_tuple<0>
{
public:
	template<typename ReturnType, typename ...TupleArgs, typename ...FunctArgs>
	inline static ReturnType unpack(const std::function<ReturnType(const TupleArgs&...)>& fun, const std::tuple<TupleArgs...>& tuple, const FunctArgs&... funargs)
	{
		return fun(funargs...);
	}
};
