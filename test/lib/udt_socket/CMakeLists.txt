cmake_minimum_required(VERSION 2.6)

project(udt_socket_test)

set (SOURCES
	test.cpp
	PooledSocketServerTest.cpp
	udt_socket_setTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/udt_socket
)

add_executable (
	udt_socket_test
	${SOURCES}
)

add_test(udt_socket_test udt_socket_test)

target_link_libraries(udt_socket_test
	udt_socket

	socket

	pthread
	udt
)

