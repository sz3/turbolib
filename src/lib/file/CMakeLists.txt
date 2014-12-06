cmake_minimum_required(VERSION 2.6)

set(SOURCES
	ConfigFile.cpp
	ConfigFile.h
	File.cpp
	File.h
	FileRemover.h
	StateSaver.h
	UseTempDirectory.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(file STATIC ${SOURCES})

