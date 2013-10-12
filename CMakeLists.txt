cmake_minimum_required(VERSION 2.6)

project ( corelib )
enable_testing()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2")
	# -ftime-report
	# -ffast-math
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

set (PROJECTS
   src/exe/heapsort
   src/exe/indexer
   src/exe/threadlock

   src/lib/algorithm
   test/lib/algorithm
   src/lib/common
   test/lib/common
   src/lib/event
   test/lib/event
   src/lib/socket
   test/lib/socket
)

include_directories(
	${corelib_SOURCE_DIR}/src/lib
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


