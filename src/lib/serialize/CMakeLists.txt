cmake_minimum_required(VERSION 2.6)

set(SOURCES
	base64.cpp
	base64.h
	define_serialize_map.h
	format.h
	str.h
	str_join.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(serialize STATIC ${SOURCES})

