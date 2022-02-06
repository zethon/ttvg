cmake_minimum_required(VERSION 3.0)

project(ttvg)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH 1)

# initialize conan libs
include(CMake/conan.cmake)
conan_cmake_run(REQUIRES
    boost/1.77.0
    sfml/2.5.1
    spdlog/1.9.2
    nlohmann_json/3.10.5
    lua/5.4.3
    BASIC_SETUP CMAKE_TARGETS
    GENERATORS cmake_find_package cmake_paths
    BUILD missing
    OPTIONS 
        boost:shared=False 
        boost:without_test=False 
        boost:without_program_options=False 
        boost:without_filesystem=False 
        boost:without_system=False 
        boost:without_exception=False
        boost:without_contract=False
        sfml:shared=True
)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS KEEP_RPATHS)

# optional configuration
option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)

# Add a RELEASE definition that we can count on regardless
# of platform
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -DRELEASE")
string(APPEND CMAKE_CXX_FLAGS_MINSIZEREL " -DRELEASE")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

if (MSVC)
    add_definitions(
        -DWIN32
        -D_WIN32
        -D_WINDOWS
        -DNOMINMAX
        -D_SCL_SECURE_NO_WARNINGS
        -DWIN32_LEAN_AND_MEAN

        # use this to supress the boost generated "warning C4996"
        # on Windows
        -D_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING
    )

    add_compile_options(
        # treat warnings as errors
        /WX
    )

    add_link_options(
        # eliminate a lot of debug linking errors
        /IGNORE:4099
    )
endif(MSVC)

# TODO: move the version information into a dedicated
# file *just* for versioning information
file(READ "src/core.h" filecontent)
string(REGEX MATCH "#define[ \t]+VERSION_MAJOR[ \t]+([0-9]*)" _ ${filecontent})
set(APP_VERSION_MAJOR ${CMAKE_MATCH_1})
string(STRIP ${APP_VERSION_MAJOR} APP_VERSION_MAJOR)
string(REGEX MATCH "#define[ \t]+VERSION_MINOR[ \t]+([0-9]*)" _ ${filecontent})
set(APP_VERSION_MINOR ${CMAKE_MATCH_1})
string(STRIP ${APP_VERSION_MINOR} APP_VERSION_MINOR)
string(REGEX MATCH "#define[ \t]+VERSION_PATCH[ \t]+([0-9]*)" _ ${filecontent})
set(APP_VERSION_PATCH ${CMAKE_MATCH_1})
string(STRIP ${APP_VERSION_PATCH} APP_VERSION_PATCH)
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.")

if (NOT "$ENV{APPVEYOR_BUILD_NUMBER}" STREQUAL "")
    set(APP_BUILD_NUMBER "$ENV{APPVEYOR_BUILD_NUMBER}")
elseif(NOT "$ENV{TRAVIS_BUILD_NUMBER}" STREQUAL "")
    set(APP_BUILD_NUMBER "$ENV{TRAVIS_BUILD_NUMBER}")
else()
    set(APP_BUILD_NUMBER "0")
endif()

add_subdirectory(src)

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
    configure_file(test-config.h.in test-config.h)
endif(BUILD_UNIT_TESTS)
