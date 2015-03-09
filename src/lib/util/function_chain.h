/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <deque>
#include <functional>

namespace turbo {
template <typename... Params>
class function_chain
{
protected:
	using Funct = std::function<void(Params...)>;

public:
	void add(Funct fun)
	{
		_chain.push_back(fun);
	}

	const Funct& finalize()
	{
		return _fun = recompute();
	}

	function_chain<Params...>& operator=(const Funct& fun)
	{
		add(fun);
		return *this;
	}

	const Funct& fun() const
	{
		return _fun;
	}

protected:
	Funct recompute()
	{
		if (_chain.empty())
			return NULL;

		std::deque<Funct>& chain(_chain);
		return [chain](Params... params)
		{
			for (auto fun : chain)
			fun(params...);
		};
	}

protected:
	std::deque<Funct> _chain;
	Funct _fun;
};
}//namespace turbo
