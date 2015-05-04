cmake_minimum_required(VERSION 2.6)

set(SOURCES
	conditional_lock_guard.h
	deadline_thread.h
	monitor.h
	thread_pool.h
	unlock_guard.h

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
