cmake_minimum_required(VERSION 2.6)

project(consistent_hashing_test)
add_definitions(-std=c++11)

set (SOURCES
	test.cpp
	circular_iteratorTest.cpp
	circular_mapTest.cpp
	hash_ringTest.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable (
	consistent_hashing_test
	${SOURCES}
)

add_test(consistent_hashing_test consistent_hashing_test)
