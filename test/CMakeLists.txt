cmake_minimum_required(VERSION 2.6)

project(vector_clock_test)
add_definitions(-std=c++11)

set (SOURCES
	test.cpp
	bounded_version_vectorTest.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable (
	vector_clock_test
	${SOURCES}
)

add_test(vector_clock_test vector_clock_test)
