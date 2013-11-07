cmake_minimum_required(VERSION 2.6)

set(SOURCES
	critbit_map.h
	critbit_tree.h
	merkle_tree.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(data_structures SOURCES ${SOURCES})

