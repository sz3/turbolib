/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <condition_variable>
#include <mutex>

namespace turbo {
class monitor
{
public:
	monitor()
		: _flag(false)
	{}

	void wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (_flag)
		{
			_flag = false;
			return;
		}
		_cv.wait(lock);
	}

	bool wait_for(unsigned ms)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (_flag)
		{
			_flag = false;
			return true;
		}
		return _cv.wait_for(lock, std::chrono::milliseconds(ms)) != std::cv_status::timeout;
	}

	void notify_one()
	{
		_cv.notify_one();
	}

	void notify_all()
	{
		_cv.notify_all();
	}

	void signal_all()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_flag = true;
		notify_all();
	}

protected:
	bool _flag;
	std::condition_variable _cv;
	std::mutex _mutex;
};
} // namespace turbo