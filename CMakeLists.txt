cmake_minimum_required(VERSION 3.0)

project(ttvg)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH 1)

# initialize conan libs
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS KEEP_RPATHS)

# optional configuration
option(UNIT_TEST "Build unit tests (default OFF)" OFF)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_subdirectory(src)

if (UNIT_TEST)
    enable_testing()
    add_subdirectory(tests)
endif (UNIT_TEST)
