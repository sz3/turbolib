cmake_minimum_required(VERSION 2.6)

project(kademlia_routing_test)
add_definitions(-std=c++11)

set (SOURCES
	test.cpp
	routing_tableTest.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_executable (
	kademlia_routing_test
	${SOURCES}
)

add_test(kademlia_routing_test kademlia_routing_test)
