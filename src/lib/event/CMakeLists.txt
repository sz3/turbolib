cmake_minimum_required(VERSION 2.6)

set(SOURCES
	CounterEvent.cpp
	CounterEvent.h
	Event.cpp
	Event.h
	Executor.h
	SchedulerThread.cpp
	SchedulerThread.h
	ThreadPool.cpp
	ThreadPool.h
	Timer.cpp
	Timer.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(event STATIC ${SOURCES})

