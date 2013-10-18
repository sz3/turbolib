cmake_minimum_required(VERSION 2.6)

set(SOURCES
	CallHistory.h
	CommandLine.cpp
	CommandLine.h
	StringUtil.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(common STATIC ${SOURCES})

