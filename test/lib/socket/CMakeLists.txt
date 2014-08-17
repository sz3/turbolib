cmake_minimum_required(VERSION 2.6)

project(socket_test)

set (SOURCES
	test.cpp
	FileByteStreamTest.cpp
	IpAddressTest.cpp
	LocalStreamSocketServerTest.cpp
	SocketWriterTest.cpp
	StatelessSocketServerTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/socket
)

add_executable (
	socket_test
	${SOURCES}
)

add_test(socket_test socket_test)

target_link_libraries(socket_test
	socket

	command_line
	event

	pthread
)

