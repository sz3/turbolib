#pragma once

#include <deque>
#include <sstream>
#include <string>

class CallHistory
{
public:
	template<class ... Types>
	void call(const std::string& fun, Types ... params)
	{
		InternalCall ic(fun);
		ic.eval(params...);
		_calls.push_back(ic.str());
	}

	std::string calls() const
	{
		using namespace std;
		std::stringstream ss;
		deque<string>::const_iterator it = _calls.begin();
		if (it != _calls.end())
			ss << *it++;
		for (; it != _calls.end(); ++it)
			ss << "|" << *it;
		return ss.str();
	}

	void clear()
	{
		_calls.clear();
	}

protected:
	class InternalCall
	{
	public:
		InternalCall(const std::string& fun)
		{
			_ss << fun << "(";
		}

		void eval()
		{
			_ss << ")";
		}

		template<class T>
		void eval(const T& first)
		{
			_ss << first << ")";
		}

		template<class T, class ... Types>
		void eval(const T& first, Types ... params)
		{
			_ss << first << ",";
			eval(params...);
		}

		std::string str() const
		{
			return _ss.str();
		}

	protected:
		std::stringstream _ss;
	};

protected:
	std::deque<std::string> _calls;
};

