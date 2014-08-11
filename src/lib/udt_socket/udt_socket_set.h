/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <set>
#include <string>

class udt_socket_set
{
public:
	enum type {
		READS = 1,
		WRITES = 2
	};

public:
	udt_socket_set(int flags);
	~udt_socket_set();
	void release();
	bool good() const;

	std::set<int> wait() const;
	bool add(int sock);
	bool remove(int sock);

	std::string lastError() const;

protected:
	void fatalError(const std::string& error);

protected:
	int _pollSet;
	int _pollFlags;

	std::string _lastError;
};
