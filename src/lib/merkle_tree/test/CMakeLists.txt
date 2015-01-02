cmake_minimum_required(VERSION 2.6)

project(merkle_tree_test)
add_definitions(-std=c++11)

set (SOURCES
	test.cpp
	critbit_mapTest.cpp
	critbit_treeTest.cpp
	merkle_treeTest.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable (
	merkle_tree_test
	${SOURCES}
)

add_test(merkle_tree_test merkle_tree_test)
