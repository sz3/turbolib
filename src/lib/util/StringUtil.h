#include <sstream>
#include <string>
#include <utility>

namespace StringUtil
{
	template <class T>
	std::string stlJoin(const T& start, const T& end, char delim=' ')
	{
		std::stringstream str;
		T it = start;
		if (it != end)
			str << *it++;
		for (; it != end; ++it)
			str << delim << *it;
		return str.str();
	}

	template <class TYPE>
	std::string stlJoin(const TYPE& container, char delim=' ')
	{
		return stlJoin(container.begin(), container.end(), delim);
	}
};

template <class T1, class T2>
std::ostream& operator<<(std::ostream& outstream, const std::pair<T1,T2>& pear)
{
	outstream << pear.first << "=" << pear.second;
	return outstream;
}
