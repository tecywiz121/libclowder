cmake_minimum_required(VERSION 2.8.12)
project(clowder)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(BUILD_SHARED_LIBS)
    add_definitions(-DCLOWDER_SHARED)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -Wall -Wextra -pedantic -Werror -std=c11")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -Wextra -pedantic -Werror -std=c++11")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
endif()

option(COVERAGE "enable code coverage flags" OFF)
option(COVERALLS "add target to upload coverage to coveralls" OFF)
option(CPPCHECK "enable scanning the source code with cppcheck" ON)

if(COVERAGE OR COVERALLS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

if (NOT BUILD_SHARED_LIBS)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(src)
