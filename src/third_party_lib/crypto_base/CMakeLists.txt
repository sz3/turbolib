cmake_minimum_required(VERSION 2.6)

set(SOURCES
	endianness.h
	tiger.c
	tiger.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(crypto_base STATIC ${SOURCES})

