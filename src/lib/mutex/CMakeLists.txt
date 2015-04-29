cmake_minimum_required(VERSION 2.6)

set(SOURCES
	conditional_lock_guard.h
	monitor.h
	unlock_guard.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(mutex SOURCES ${SOURCES})

