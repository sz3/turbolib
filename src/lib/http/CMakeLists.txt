cmake_minimum_required(VERSION 2.6)

set(SOURCES
	HttpParser.cpp
	HttpParser.h
	Url.cpp
	Url.h

	http_parser/http_parser.c
	http_parser/http_parser.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(http STATIC ${SOURCES})

