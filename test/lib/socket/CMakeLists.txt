cmake_minimum_required(VERSION 2.6)

project(socket_test)

set (SOURCES
	test.cpp
	FileByteStreamTest.cpp
	SimplePoolTest.cpp
	SocketWriterTest.cpp
	StatelessSocketServerTest.cpp
	StreamSocketAcceptorServerTest.cpp
	TcpAcceptorServerTest.cpp
	socket_addressTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/socket
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
	tbb
)

