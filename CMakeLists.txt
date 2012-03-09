cmake_minimum_required(VERSION 2.6)

project ( cmake )
enable_testing()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2")
	# -ftime-report
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

set (PROJECTS
   src/exe/pdevkit
   src/lib/common
   src/lib/graphics
   test/lib/common
)

include_directories(
	${cmake_SOURCE_DIR}/src/lib
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


