#pragma once

#include <condition_variable>
#include <mutex>

class Event
{
public:
	Event();

	void signal();
	bool wait(unsigned millis = ~0) const;

protected:
	mutable std::mutex _mutex;
	mutable std::condition_variable _cond;
};
//*/
/*
#include <pthread.h>

class Event
{
public:
	Event() {}

	bool wait(unsigned foo=0) const
	{
		pthread_mutex_lock(&m);
		pthread_cond_wait(&cv, &m);
		pthread_mutex_unlock(&m);
		return true;
	}

	void signal()
	{
		pthread_mutex_lock(&m);
		pthread_cond_signal(&cv);
		pthread_mutex_unlock(&m);
	}

protected:
	mutable pthread_cond_t cv;
	mutable pthread_mutex_t m;
};
//*/
