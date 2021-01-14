cmake_minimum_required(VERSION 3.19)

project(qcpc)

add_library(qcpc INTERFACE)
target_include_directories(qcpc INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(qcpc INTERFACE cxx_std_20)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests ${PROJECT_SOURCE_DIR}/tests/tests.cpp)
target_link_libraries(tests GTest::GTest GTest::Main qcpc)
gtest_discover_tests(tests)
