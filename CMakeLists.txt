cmake_minimum_required(VERSION 2.8)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

project(curi)

set(CURI_VERSION_MAJOR 1)
set(CURI_VERSION_MINOR 0)

add_subdirectory(src)

add_subdirectory(doc)

enable_testing()
add_subdirectory(tests)