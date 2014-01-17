cmake_minimum_required(VERSION 2.6)

set(SOURCES
	Timer.cpp
	Timer.h
	WaitFor.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(time STATIC ${SOURCES})

