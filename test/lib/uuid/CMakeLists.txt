cmake_minimum_required(VERSION 2.6)

project(uuid_test)

set (SOURCES
	test.cpp
	UuidGeneratorTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/uuid
)

add_executable (
	uuid_test
	${SOURCES}
)

add_test(uuid_test uuid_test)

