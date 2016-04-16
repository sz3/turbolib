cmake_minimum_required(VERSION 2.6)

set(SOURCES
	critbit_map.h
	critbit_tree.h
	merkle_tree.h
)

add_custom_target(merkle_tree SOURCES ${SOURCES})

add_subdirectory(test)
