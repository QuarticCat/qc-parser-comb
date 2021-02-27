cmake_minimum_required(VERSION 3.19)

project(qcpc2)

add_library(qcpc2 INTERFACE)
target_include_directories(qcpc2 INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(qcpc2 INTERFACE cxx_std_20)

enable_testing()
find_package(GTest REQUIRED)

file(GLOB_RECURSE test_sources
        ${PROJECT_SOURCE_DIR}/tests/*.cpp
        ${PROJECT_SOURCE_DIR}/examples/*.cpp)
add_executable(tests ${test_sources})
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main qcpc2)
if (MSVC)
    target_compile_options(tests PRIVATE /W4 /WX /utf-8)
else ()
    target_compile_options(tests PRIVATE -pedantic -Wall -Wextra -Werror -fno-exceptions -fno-rtti)
endif ()
gtest_discover_tests(tests)
