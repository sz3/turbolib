cmake_minimum_required(VERSION 2.6)

project(udt_socket_test)

set (SOURCES
	test.cpp
	UdtServerTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/udt_socket
)

add_executable (
	udt_socket_test
	${SOURCES}
)

add_test(udt_socket_test udt_socket_test)

target_link_libraries(udt_socket_test
	udt_socket

	command_line
	event
	socket
	time

	pthread
	tbb
	udt
)

