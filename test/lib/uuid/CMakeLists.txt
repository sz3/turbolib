cmake_minimum_required(VERSION 2.6)

project(uuid_test)

set (SOURCES
	test.cpp
	UuidGeneratorTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/uuid
)

add_executable (
	uuid_test
	${SOURCES}
)

add_test(uuid_test uuid_test)

