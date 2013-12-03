cmake_minimum_required(VERSION 2.6)

project(event_test)

set (SOURCES
	test.cpp
	EventTest.cpp
	SchedulerThreadTest.cpp
	ThreadPoolTest.cpp
)

include_directories(
	${corelib_SOURCE_DIR}/test
	${corelib_SOURCE_DIR}/test/lib
	${corelib_SOURCE_DIR}/src/lib/event
)

add_executable (
	event_test
	${SOURCES}
)

add_test(event_test event_test)

target_link_libraries(event_test
	event
	
	pthread
	tbb
)

