cmake_minimum_required(VERSION 2.6)

set(SOURCES
	stopwatch.h
	WaitFor.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(time SOURCES ${SOURCES})
