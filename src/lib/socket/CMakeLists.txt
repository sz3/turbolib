cmake_minimum_required(VERSION 2.6)

set(SOURCES
	ConnectionByteStream.cpp
	ConnectionByteStream.h
	IByteStream.h
	LocalStreamSocketServer.cpp
	LocalStreamSocketServer.h
	UdpServer.cpp
	UdpServer.h
	UdpSocket.cpp
	UdpSocket.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(socket STATIC ${SOURCES})

target_link_libraries(socket
	event
)
