cmake_minimum_required(VERSION 2.6)

project(data_structures_test)

set (SOURCES
	test.cpp
	critbit_mapTest.cpp
	critbit_treeTest.cpp
	merkle_treeTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/data_structures
)

add_executable (
	data_structures_test
	${SOURCES}
)

add_test(data_structures_test data_structures_test)

target_link_libraries(data_structures_test
	pthread
)

