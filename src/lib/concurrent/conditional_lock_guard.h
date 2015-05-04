/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

namespace turbo {
template <typename M>
class conditional_lock_guard
{
public:
	conditional_lock_guard(M& myMutex, bool shouldLock)
		: _mutex(myMutex)
		, _shouldLock(shouldLock)
	{
		if (_shouldLock)
			_mutex.lock();
	}

	~conditional_lock_guard()
	{
		if (_shouldLock)
			_mutex.unlock();
	}

protected:
	M& _mutex;
	bool _shouldLock;
};
} // namespace turbo
