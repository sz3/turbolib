cmake_minimum_required(VERSION 2.6)

set(SOURCES
	Event.cpp
	Event.h
	Timer.cpp
	Timer.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(event STATIC ${SOURCES})

