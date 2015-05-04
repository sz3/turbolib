cmake_minimum_required(VERSION 2.6)

project ( turbolib )
enable_testing()

set(BUILD_HTTP "TRUE" CACHE BOOL "build http lib")
set(EXTRA_INCLUDE_DIRS "" CACHE STRING "any extra directories you want on the include path (ex: wacky library locations)")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
	set(CMAKE_C_FLAGS "${CFLAGS} -Wall -std=c99")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O2 -fPIC -std=c++11")
	# -ftime-report
	# -ffast-math
endif()

set (PROJECTS
   src/lib/concurrent
   test/lib/concurrent
   src/lib/crypto
   test/lib/crypto
   src/lib/file
   test/lib/file
   src/lib/serialize
   test/lib/serialize
   src/lib/socket
   test/lib/socket
   src/lib/system
   test/lib/system
   src/lib/time
   src/lib/udt_socket
   test/lib/udt_socket
   src/lib/util
   test/lib/util

   src/lib/consistent_hashing
   src/lib/merkle_tree
   src/lib/vector_clock
   src/third_party_lib/cppformat
   src/third_party_lib/crypto_base
)

if (BUILD_HTTP)
   set (PROJECTS
      ${PROJECTS}
      src/lib/http
      test/lib/http
   )
endif()

include_directories(
	${turbolib_SOURCE_DIR}/src/lib
	${turbolib_SOURCE_DIR}/src/third_party_lib
	${EXTRA_INCLUDE_DIRS}
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


