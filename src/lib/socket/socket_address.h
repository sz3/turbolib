/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <sstream>
#include <string>

class socket_address
{
public:
	socket_address();
	socket_address(std::string destination);
	socket_address(std::string address, unsigned short port);

	const std::string& address() const;
	unsigned short port() const;

	bool operator==(const socket_address& other) const;
	std::string toString() const;
	bool fromString(const std::string& str);

protected:
	std::string _address;
	unsigned short _port;
};

inline socket_address::socket_address()
	: _port(0)
{
}

inline socket_address::socket_address(std::string destination)
	: _address(std::move(destination))
	, _port(0)
{
}

inline socket_address::socket_address(std::string address, unsigned short port)
	: _address(std::move(address))
	, _port(port)
{
}

inline const std::string& socket_address::address() const
{
	return _address;
}

inline unsigned short socket_address::port() const
{
	return _port;
}

inline bool socket_address::operator==(const socket_address& other) const
{
	return _address == other._address && _port == other._port;
}

inline std::string socket_address::toString() const
{
	std::stringstream str;
	str << _address << ":" << _port;
	return str.str();
}

inline bool socket_address::fromString(const std::string& str)
{
	size_t sep = str.find_last_of(':');
	if (sep == std::string::npos || sep >= str.size()-1)
		return false;

	_address = str.substr(0, sep);
	_port = std::stoi(str.substr(sep+1));
	return true;
}

namespace std {
	template <>
	struct hash<socket_address>
	{
		std::size_t operator()(const socket_address& addr) const
		{
			using std::string;
			return hash<string>()(addr.address()) xor hash<unsigned short>()(addr.port());
		}
	};
}
