cmake_minimum_required(VERSION 2.6)

set(SOURCES
	routing_table.h
)

add_custom_target(kademlia_routing SOURCES ${SOURCES})

add_subdirectory(test)
