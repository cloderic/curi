cmake_minimum_required(VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

project(curi)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)