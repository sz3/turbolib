cmake_minimum_required(VERSION 2.6)

set(SOURCES
	FileByteStream.cpp
	FileByteStream.h
	IByteStream.h
	ISocketServer.h
	ISocketWriter.h
	IpAddress.cpp
	IpAddress.h
	LocalStreamSocketServer.cpp
	LocalStreamSocketServer.h
	MockSocketServer.h
	MockSocketWriter.h
	PooledSocketServer.h
	SocketWriter.h
	StatelessSocketServer.h
	StringByteStream.cpp
	StringByteStream.h
	udp_socket.cpp
	udp_socket.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(socket STATIC ${SOURCES})

