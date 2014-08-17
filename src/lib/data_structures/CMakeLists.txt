cmake_minimum_required(VERSION 2.6)

set(SOURCES
	bounded_version_vector.h
	circular_iterator.h
	critbit_map.h
	critbit_tree.h
	merkle_location.h
	merkle_tree.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(data_structures SOURCES ${SOURCES})

