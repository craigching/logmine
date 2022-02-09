cmake_minimum_required(VERSION 3.16)

project(logmine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(logmine src/logmine.cpp)

target_include_directories(logmine PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/lib/sajson/include")
#target_link_libraries(logmine tbb)
target_compile_options(logmine PRIVATE -Wno-unknown-warning-option -Wno-tautological-compare -Wno-sign-compare -D_REENTRANT -Wno-ignored-attributes -O3 -DBOOST_DISABLE_ASSERTS)

find_package(Catch2 REQUIRED)

add_executable(logmine_tests tests/src/logmine_tests.cpp)
target_include_directories(logmine_tests PUBLIC /usr/local/Cellar/catch2/2.13.4/include "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(logmine_tests PRIVATE Catch2::Catch2)

add_executable(align_tests tests/src/align_tests.cpp)
target_include_directories(align_tests PUBLIC /usr/local/Cellar/catch2/2.13.4/include "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(align_tests PRIVATE Catch2::Catch2)

add_executable(tokens_tests tests/src/tokens_tests.cpp)
target_include_directories(tokens_tests PUBLIC /usr/local/Cellar/catch2/2.13.4/include "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(tokens_tests PRIVATE Catch2::Catch2)
