cmake_minimum_required(VERSION 2.6)

project(util_test)

set (SOURCES
	test.cpp
	CallHistoryTest.cpp
	function_chainTest.cpp
	randomTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/util
)

add_executable (
	util_test
	${SOURCES}
)

add_test(util_test util_test)

target_link_libraries(util_test
	serialize
)

