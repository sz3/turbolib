
#include "event/Event.h"
#include "time/Timer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <pthread.h>
#include <unistd.h>
using namespace std;


/*class Event
{
public:
	Event() {}

	void wait()
	{
		pthread_mutex_lock(&m);
		pthread_cond_wait(&cv, &m);
		pthread_mutex_unlock(&m);
	}

	void signal()
	{
		pthread_mutex_lock(&m);
		pthread_cond_signal(&cv);
		pthread_mutex_unlock(&m);
	}

protected:
	pthread_cond_t cv;
	pthread_mutex_t m;
};*/

Event theEvent;

class EventScience
{
public:
	EventScience(const Timer& startup, int num)
		: _timer(NULL)
		, _thread( bind(&EventScience::testEventFun, this, ref(startup), num) )
	{
	}

	void testEventFun(const Timer& startup, int num)
	{
		_results << num << " time taken to start thread: " << startup.micros() << "us, " << startup.millis() << "ms" << std::endl;

		theEvent.wait();

		if (_timer != NULL)
			_results << num << " time taken for event to propagate: " << _timer->micros() << "us, " << _timer->millis() << "ms" << std::endl;
	}

	void setTimer(const Timer* timer)
	{
		_timer = timer;
	}

	void stop_and_wait()
	{
		_thread.join();
	}

	std::string results() const
	{
		return _results.str();
	}

protected:
	const Timer* _timer;
	std::thread  _thread;
	std::stringstream  _results;
};

int main()
{
	std::cerr << "hi" << std::endl;

	Timer startup;
	EventScience science1(startup, 1);

	::sleep(1);
	std::cerr << "hi." << std::endl;
	Timer t;

	science1.setTimer(&t);

	theEvent.signal();

	science1.stop_and_wait();
	std::cout << science1.results() << std::endl;

	return 0;
}
