cmake_minimum_required(VERSION 3.20)
project(TurboQ VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################
# Library type
option(BUILD_SHARED "Build as shared library" OFF)

if(BUILD_SHARED)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

################################################
# Sources and headers
file(GLOB_RECURSE ENGINE_HEADERS include/*.hpp)
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)

add_library(turboq ${LIB_TYPE} ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(turboq PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${ENGINE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${ENGINE_SOURCES})

################################################
# Unit tests
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
