cmake_minimum_required(VERSION 2.6)

project(serialize_test)

set (SOURCES
	test.cpp
	MessagePackTest.cpp
	StringUtilTest.cpp
	define_load_mapTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/serialize
)

add_executable (
	serialize_test
	${SOURCES}
)

add_test(serialize_test serialize_test)

target_link_libraries(serialize_test
	serialize

	msgpack
)

