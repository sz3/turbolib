cmake_minimum_required(VERSION 2.6)

project(system_test)

set (SOURCES
	test.cpp
	popenTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/system
)

add_executable (
	system_test
	${SOURCES}
)

add_test(system_test system_test)

