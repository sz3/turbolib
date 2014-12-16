cmake_minimum_required(VERSION 2.6)

set(SOURCES
	bounded_version_vector.h
)

add_custom_target(vector_clock SOURCES ${SOURCES})

add_subdirectory(test)
