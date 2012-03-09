cmake_minimum_required(VERSION 2.6)

project(common_test)

set (SOURCES
   test.cpp
   TestFaceTest.cpp
   ThreadTest.cpp
)

include_directories(
	${cmake_SOURCE_DIR}/test
	${cmake_SOURCE_DIR}/test/lib
	${cmake_SOURCE_DIR}/src/lib/common
)

add_executable (
   common_test
   ${SOURCES} # We could've listed the source files here directly instead of using a variable to store them
)

add_test(common_test common_test)

target_link_libraries(common_test
	common
	
	pthread
)

