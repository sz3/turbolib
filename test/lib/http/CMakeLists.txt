cmake_minimum_required(VERSION 2.6)

project(http_test)

set (SOURCES
	test.cpp
	HttpByteStreamTest.cpp
	HttpParserTest.cpp
	HttpResponseTest.cpp
	StatusCodeTest.cpp
	UrlTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/http
)

add_executable (
	http_test
	${SOURCES}
)

add_test(http_test http_test)

target_link_libraries(http_test
	http
	serialize
	socket
)

