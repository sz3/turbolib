#include "MultiThreadedExecutor.h"

#include <iostream>

MultiThreadedExecutor::MultiThreadedExecutor(unsigned numThreads)
	: _running(false)
	, _numThreads(numThreads)
	, _notifyRunning(numThreads)
{
}

MultiThreadedExecutor::~MultiThreadedExecutor()
{
	stop();
}


bool MultiThreadedExecutor::start()
{
	if (_running)
		return true;
	_running = true;

	for (unsigned i = 0; i < _numThreads; ++i)
		_threads.push_back( std::thread(std::bind(&MultiThreadedExecutor::run, this)) );
	return _running = _notifyRunning.wait(5000);
}

void MultiThreadedExecutor::stop()
{
	_running = false;
	_notifyWork.shutdown();
	for (std::list<std::thread>::iterator it = _threads.begin(); it != _threads.end(); ++it)
	{
		if (it->joinable())
			it->join();
	}
	_threads.clear();
}

void MultiThreadedExecutor::execute(std::function<void()> fun)
{
	_queue.push(fun);
	_notifyWork.signalOne();
}

size_t MultiThreadedExecutor::queued() const
{
	return _queue.unsafe_size();
}

void MultiThreadedExecutor::run()
{
	_notifyRunning.signal();
	while (_running)
	{
		_notifyWork.wait();
		if (!_running)
			break;

		std::function<void()> fun;
		while (_queue.try_pop(fun))
			fun();
	}
}
