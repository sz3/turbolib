cmake_minimum_required(VERSION 2.6)

set(SOURCES
	circular_iterator.h
	circular_map.h
	hash_ring.h
)

add_custom_target(consistent_hashing SOURCES ${SOURCES})

add_subdirectory(test)
