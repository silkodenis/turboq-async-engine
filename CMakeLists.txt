cmake_minimum_required(VERSION 3.20)
project(TurboQ VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################

option(BUILD_SHARED "Build as shared library" OFF)

if(BUILD_SHARED)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

################################################

file(GLOB_RECURSE ENGINE_HEADERS include/*.hpp)
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)

add_library(turboq ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(turboq PUBLIC include)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${ENGINE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_SOURCES})

################################################

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
