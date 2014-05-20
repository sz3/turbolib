cmake_minimum_required(VERSION 2.6)

set(SOURCES
	IHttpByteStream.h
	HttpByteStream.cpp
	HttpByteStream.h
	HttpParser.cpp
	HttpParser.h
	HttpResponse.cpp
	HttpResponse.h
	StatusCode.h
	Url.cpp
	Url.h

	http_parser/http_parser.c
	http_parser/http_parser.c
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(http STATIC ${SOURCES})

