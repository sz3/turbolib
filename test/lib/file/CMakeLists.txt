cmake_minimum_required(VERSION 2.6)

project(file_test)

set (SOURCES
	test.cpp
	ConfigFileTest.cpp
	FileTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/file
)

add_executable (
	file_test
	${SOURCES}
)

add_test(file_test file_test)

target_link_libraries(file_test
	file

	serialize
)

