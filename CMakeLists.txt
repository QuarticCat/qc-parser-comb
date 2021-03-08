cmake_minimum_required(VERSION 3.19)

project(qcpc)

add_library(qcpc INTERFACE)
target_include_directories(qcpc INTERFACE include)
target_compile_features(qcpc INTERFACE cxx_std_20)

set(QCPC_IS_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(QCPC_IS_MAIN_PROJECT ON)
endif ()

option(QCPC_BUILD_TESTS "Build tests" ${QCPC_IS_MAIN_PROJECT})
if (QCPC_BUILD_TESTS)
    enable_testing()

    find_package(GTest REQUIRED)

    file(GLOB_RECURSE test_sources
            tests/*.cpp
            examples/*.cpp)
    add_executable(qcpc_tests ${test_sources})
    target_link_libraries(qcpc_tests PRIVATE
            GTest::GTest
            GTest::Main
            qcpc)
    if (MSVC)
        target_compile_options(qcpc_tests PRIVATE /W4 /WX /utf-8)
    else ()
        target_compile_options(qcpc_tests PRIVATE -pedantic -Wall -Wextra -Werror -fno-exceptions -fno-rtti)
    endif ()

    gtest_discover_tests(qcpc_tests)
endif ()
