cmake_minimum_required(VERSION 2.6)

project(util_test)

set (SOURCES
	test.cpp
	CallHistoryTest.cpp
	merkle_treeTest.cpp
	RandomTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/util
)

add_executable (
	util_test
	${SOURCES}
)

add_test(util_test util_test)

target_link_libraries(util_test
)

