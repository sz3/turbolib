cmake_minimum_required(VERSION 2.6)

project(crypto_test)

set (SOURCES
	test.cpp
	tiger_hashTest.cpp
)

include_directories(
	${turbolib_SOURCE_DIR}/test
	${turbolib_SOURCE_DIR}/test/lib
	${turbolib_SOURCE_DIR}/src/lib/crypto
)

add_executable (
	crypto_test
	${SOURCES}
)

add_test(crypto_test crypto_test)

target_link_libraries(crypto_test
	crypto_base
	serialize
)
