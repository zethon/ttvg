cmake_minimum_required(VERSION 3.0)

project(ttvg)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH 1)

# optional configuration
option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)
option(SHARED_SFML "Use shared SFML libraries (default OFF)" ON)

set(SHARED_SFML_STR "False")
if (SHARED_SFML)
    set(SHARED_SFML_STR "True")
endif(SHARED_SFML)

# initialize conan libs
include(CMake/conan.cmake)
conan_cmake_run(REQUIRES
    boost/1.84.0
    ogg/1.3.5
    sfml/2.6.1
    spdlog/1.14.1
    nlohmann_json/3.11.3
    lua/5.4.3
    zlib/1.3.1
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
        sfml:shared=${SHARED_SFML_STR}
)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS KEEP_RPATHS)

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
        -D_WIN32_WINDOWS
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

# get the git commit hash
execute_process(COMMAND git log --pretty=format:%h -n 1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_REV
                ERROR_QUIET)

if (NOT "${GIT_REV}" STREQUAL "")
    set(APP_BUILD_NUMBER "${GIT_REV}")
else()
    set(APP_BUILD_NUMBER "0")
endif()

add_subdirectory(date)
include_directories(${CMAKE_SOURCE_DIR}/date/include)

# include TGUI and build only what we need
set(TGUI_BUILD_GUI_BUILDER FALSE)
set(TGUI_BUILD_EXAMPLES FALSE)
set(TGUI_BUILD_TESTS FALSE)
set(TGUI_STATIC_LIBRARIES TRUE)
set(TGUI_SHARED_LIBS FALSE)
set(TGUI_BUILD_FRAMEWORK FALSE)
add_subdirectory(TGUI)
include_directories(TGUI/include)

add_subdirectory(src)

if(BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
    configure_file(test-config.h.in test-config.h)
endif(BUILD_UNIT_TESTS)
