cmake_minimum_required(VERSION 2.6)

project(serialize_test)

set (SOURCES
	test.cpp
	StringUtilTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/serialize
)

add_executable (
	serialize_test
	${SOURCES}
)

add_test(serialize_test serialize_test)

target_link_libraries(serialize_test
	serialize
)

