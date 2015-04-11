cmake_minimum_required(VERSION 2.6)

project(serialize_test)

set (SOURCES
	test.cpp
	base64Test.cpp
	define_serialize_mapTest.cpp
	MessagePackTest.cpp
	strTest.cpp
	str_joinTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
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

