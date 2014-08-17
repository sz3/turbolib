cmake_minimum_required(VERSION 2.6)

set(SOURCES
	UdtEpoll.cpp
	UdtEpoll.h
	UdtScope.cpp
	UdtScope.h
	UdtSocket.cpp
	UdtSocket.h
	udt_socket.cpp
	udt_socket.h
	udt_socket_set.cpp
	udt_socket_set.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(udt_socket STATIC ${SOURCES})

