cmake_minimum_required(VERSION 3.0.2)

project(Modulo C)

# set(CMAKE_VERSBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (APPLE)
	set(CMAKE_MACOSX_RPATH ON)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

add_subdirectory(src/modulo)
add_subdirectory(src/terminal)
