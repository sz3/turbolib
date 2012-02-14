cmake_minimum_required(VERSION 2.6)

project ( platformdevkit )

set (PROJECTS
   src/exe/pdevkit
   src/lib/common
   src/lib/graphics
)

include_directories(
	${platformdevkit_SOURCE_DIR}/src/lib
)

foreach(proj ${PROJECTS})
	add_subdirectory(${proj} build/${proj})
endforeach()


