cmake_minimum_required(VERSION 2.6)

set(SOURCES
	StringUtil.cpp
	StringUtil.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(serialize STATIC ${SOURCES})

