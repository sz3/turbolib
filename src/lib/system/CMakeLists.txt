cmake_minimum_required(VERSION 2.6)

set(SOURCES
	popen.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(system SOURCES ${SOURCES})

