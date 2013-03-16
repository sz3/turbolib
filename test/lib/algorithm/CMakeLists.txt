cmake_minimum_required(VERSION 2.6)

project(algorithm_test)

set (SOURCES
	test.cpp
	SortCompareTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/algorithm
)

add_executable (
	algorithm_test
	${SOURCES}
)

add_test(algorithm_test algorithm_test)

target_link_libraries(algorithm_test
	event
)

