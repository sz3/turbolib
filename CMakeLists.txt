cmake_minimum_required(VERSION 2.6)

project ( cmake )

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


