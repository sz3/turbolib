cmake_minimum_required(VERSION 2.6)

set(SOURCES
	FileByteStream.cpp
	FileByteStream.h
	IByteStream.h
	IIpSocket.h
	IPacketServer.h
	IpAddress.cpp
	IpAddress.h
	LocalStreamSocketServer.cpp
	LocalStreamSocketServer.h
	UdpClientSocket.cpp
	UdpClientSocket.h
	UdpServer.cpp
	UdpServer.h
	UdpSocket.cpp
	UdpSocket.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(socket STATIC ${SOURCES})

