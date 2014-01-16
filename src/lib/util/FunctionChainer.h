/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <deque>
#include <functional>

template <typename... Params>
class FunctionChainer
{
public:
	using Funct = std::function<void(Params...)>;

public:
	FunctionChainer(Funct fun)
	{
		if (fun)
			_funs.push_back(fun);
	}

	void add(Funct fun)
	{
		_funs.push_back(fun);
	}

	Funct generate()
	{
		if (_funs.empty())
			return NULL;

		std::deque<Funct> funs(std::move(_funs));
		return [funs](Params... params)
		{
			for (auto fun : funs)
				fun(params...);
		};
	}

protected:
	std::deque<Funct> _funs;
};
