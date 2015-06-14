cmake_minimum_required(VERSION 2.6)

set(SOURCES
	CallHistory.h
	cstrfun.h
	function_chain.h
	random.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(util SOURCES ${SOURCES})

