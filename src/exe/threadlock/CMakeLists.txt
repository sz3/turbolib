cmake_minimum_required(VERSION 2.6)

project(threadlock)

set (SOURCES
	threadlock.cpp
)

add_executable (
	threadlock
	${SOURCES} # We could've listed the source files here directly instead of using a variable to store them
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(threadlock
	event
	time

	pthread
)

