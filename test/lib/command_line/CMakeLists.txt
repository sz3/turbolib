cmake_minimum_required(VERSION 2.6)

project(command_line_test)

set (SOURCES
	test.cpp
	CommandLineTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/command_line
)

add_executable (
	command_line_test
	${SOURCES}
)

add_test(command_line_test command_line_test)

target_link_libraries(command_line_test
	command_line
)

