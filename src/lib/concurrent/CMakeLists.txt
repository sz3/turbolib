cmake_minimum_required(VERSION 2.6)

set(SOURCES
	deadline_thread.h
	thread_pool.h
	DeadlineScheduler.h
	IExecutor.h
	IScheduler.h
	SimpleExecutor.h
	ThreadPoolExecutor.h
	ThreadSpawningExecutor.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(concurrent SOURCES ${SOURCES})
