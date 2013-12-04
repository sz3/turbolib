cmake_minimum_required(VERSION 2.6)

set(SOURCES
	CounterEvent.cpp
	CounterEvent.h
	Event.cpp
	Event.h
	IExecutor.h
	MultiThreadedExecutor.cpp
	MultiThreadedExecutor.h
	SchedulerThread.cpp
	SchedulerThread.h
	SimpleExecutor.h
	ThreadSpawningExecutor.h
	Timer.cpp
	Timer.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(event STATIC ${SOURCES})

