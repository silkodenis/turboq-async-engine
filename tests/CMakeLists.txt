add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/Catch2 Catch2_build)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(turboq_tests ${TEST_SOURCES})

target_include_directories(turboq_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(turboq_tests PRIVATE turboq Catch2::Catch2WithMain)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SOURCES})

if(NOT "${CMAKE_GENERATOR}" MATCHES "Xcode")
    include(CTest)
    include(Catch)
    catch_discover_tests(turboq_tests)
endif()
