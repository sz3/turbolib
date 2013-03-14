cmake_minimum_required(VERSION 2.6)

project(algorithm)

set (SOURCES
	NovelMaxHeap.h
	heapsort.h
	mergesort.h
)

add_custom_target(algorithm SOURCES ${SOURCES})

