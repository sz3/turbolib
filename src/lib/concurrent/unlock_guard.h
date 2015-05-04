/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

namespace turbo {
template <typename M>
class unlock_guard
{
public:
	unlock_guard(M& myMutex)
		: _mutex(myMutex)
	{
		_mutex.unlock();
	}

	~unlock_guard()
	{
		_mutex.lock();
	}

protected:
	M& _mutex;
};
} // namespace turbo
