project(tests)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

set(PROJECT_FILES
    ../src/AnimatedSprite.cpp
    ../src/AnimatedSprite.h

    ../src/Background.cpp
    ../src/Background.h

    ../src/Intersection.cpp
    ../src/Intersection.h

    ../src/ResourceManager.cpp
    ../src/ResourceManager.h

    ../src/TTUtils.cpp
    ../src/TTUtils.h

    ../src/Vehicle.cpp
    ../src/Vehicle.h
)

add_executable(test_misc
    main.cpp
    test_misc.cpp
    ${PROJECT_FILES}
)

target_link_libraries(test_misc
    PUBLIC
        ${CONAN_LIBS}
)
