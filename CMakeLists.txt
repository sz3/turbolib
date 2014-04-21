cmake_minimum_required(VERSION 2.6)

project ( corelib )
enable_testing()

set(BUILD_HTTP "TRUE" CACHE BOOL "build http lib")
set(BUILD_UUID "TRUE" CACHE BOOL "build uuid lib (uses boost)")
set(EXTRA_INCLUDE_DIRS "" CACHE STRING "any extra directories you want on the include path (ex: wacky library locations)")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -fPIC")
	# -ftime-report
	# -ffast-math
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    # gdwarf-3 to help older gdbs
    set(CMAKE_CXX_FLAGS "-std=c++0x -flto -gdwarf-3")
endif()

set (PROJECTS
   src/lib/command_line
   test/lib/command_line
   src/lib/data_structures
   test/lib/data_structures
   src/lib/event
   test/lib/event
   src/lib/file
   test/lib/file
   src/lib/serialize
   test/lib/serialize
   src/lib/socket
   test/lib/socket
   src/lib/time
   src/lib/udt_socket
   test/lib/udt_socket
   src/lib/util
   test/lib/util
)

if (BUILD_HTTP)
   set (PROJECTS
      ${PROJECTS}
      src/lib/http
      test/lib/http
   )
endif()

if (BUILD_UUID)
   set (PROJECTS
      ${PROJECTS}
      src/lib/uuid
      test/lib/uuid
   )
endif()

include_directories(
	${corelib_SOURCE_DIR}/src/lib
	${EXTRA_INCLUDE_DIRS}
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


