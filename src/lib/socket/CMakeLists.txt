cmake_minimum_required(VERSION 2.6)

set(SOURCES
	FileByteStream.cpp
	FileByteStream.h
	IByteStream.h
	ISocketPool.h
	ISocketServer.h
	ISocketWriter.h
	LocalStreamSocketServer.h
	MockSocketServer.h
	MockSocketWriter.h
	NullByteStream.h
	PooledSocketServer.h
	SimplePool.h
	SocketWriter.h
	StatelessSocketServer.h
	StreamSocketAcceptorServer.h
	StringByteStream.cpp
	StringByteStream.h
	local_stream_socket.cpp
	local_stream_socket.h
	mock_socket.h
	socket_address.h
	tcp_socket.cpp
	tcp_socket.h
	udp_socket.cpp
	udp_socket.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(socket STATIC ${SOURCES})

