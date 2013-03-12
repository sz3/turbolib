cmake_minimum_required(VERSION 2.6)

project(heapsort)

set (SOURCES
	NovelMaxHeap.h
	heapsort.h
	main.cpp
)

add_executable (
	heapsort
	${SOURCES}
)

target_link_libraries(heapsort
	event
)

