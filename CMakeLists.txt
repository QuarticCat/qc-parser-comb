cmake_minimum_required(VERSION 3.19)

project(qcpc)

add_library(qcpc INTERFACE)
target_include_directories(qcpc INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(qcpc INTERFACE cxx_std_20)

enable_testing()
find_package(GTest REQUIRED)

file(GLOB_RECURSE test_sources ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(tests ${test_sources})
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main qcpc)
if (MSVC)
    target_compile_options(tests PRIVATE /W4 /WX /utf-8)
else ()
    target_compile_options(tests PRIVATE -pedantic -Wall -Wextra -Werror -fno-exceptions -fno-rtti)
endif ()
gtest_discover_tests(tests)
