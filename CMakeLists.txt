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
   src/exe/pdevkit

   src/lib/common
   test/lib/common
   src/lib/event
   test/lib/event
   src/lib/socket
   test/lib/socket

   src/lib/graphics
)

include_directories(
	${corelib_SOURCE_DIR}/src/lib
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


