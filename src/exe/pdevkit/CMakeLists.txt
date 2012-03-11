cmake_minimum_required(VERSION 2.6)

project(pdevkit)

set (SOURCES
	main.cpp
)

add_executable (
	pdevkit
	${SOURCES} # We could've listed the source files here directly instead of using a variable to store them
)

target_link_libraries(pdevkit
	common
	graphics
	
	crypto
)

