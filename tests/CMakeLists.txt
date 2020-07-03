project(tests)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_FILES
    ../src/Background.cpp
    ../src/Background.h
)

add_executable(test_misc
    main.cpp
    test_misc.cpp
    ${BBSPIRIT_FILES}
)

target_link_libraries(test_misc
    PUBLIC
        ${CONAN_LIBS}
)
