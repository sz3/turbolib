cmake_minimum_required(VERSION 2.6)

set(SOURCES
	UdtEpoll.cpp
	UdtEpoll.h
	UdtScope.cpp
	UdtScope.h
	UdtServer.cpp
	UdtServer.h
	UdtSocket.cpp
	UdtSocket.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(udt_socket STATIC ${SOURCES})

