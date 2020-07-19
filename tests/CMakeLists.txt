project(tests)

function(add_test testname FILES)
    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
        ${FILES}
    )

    target_link_libraries(test_${testname}
        PUBLIC
            ${CONAN_LIBS}
    )
endfunction(add_test)

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

    ../src/VehicleFactory.cpp
    ../src/VehicleFactory.h
)

add_test("misc" "${PROJECT_FILES}")
add_test("background" "${PROJECT_FILES}")
