cmake_minimum_required(VERSION 2.6)

project(concurrent_test)

set (SOURCES
	test.cpp
	deadline_threadTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/concurrent
)

add_executable (
	concurrent_test
	${SOURCES}
)

add_test(concurrent_test concurrent_test)

target_link_libraries(concurrent_test

	pthread
)

