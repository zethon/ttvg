project(tests)

function(create_test testname FILES)

    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
        ${FILES}
    )

    target_link_libraries(test_${testname}
        PUBLIC
            ${CONAN_LIBS}
    )

    add_test(NAME test_${testname}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction(create_test)

set(PROJECT_FILES
    ../src/AnimatedSprite.h    
    ../src/AnimatedSprite.cpp
    
    ../src/Background.h
    ../src/Background.cpp
    
    ../src/Intersection.h
    ../src/Intersection.cpp
    
    ../src/PathFactory.h
    ../src/PathFactory.cpp

    ../src/ResourceManager.h
    ../src/ResourceManager.cpp

    ../src/TransitionPoint.h
    ../src/TransitionPoint.cpp

    ../src/TTUtils.h
    ../src/TTUtils.cpp

    ../src/Vehicle.h
    ../src/Vehicle.cpp
    
    ../src/VehicleFactory.h
    ../src/VehicleFactory.cpp
)

create_test("misc" "${PROJECT_FILES}")
create_test("background" "${PROJECT_FILES}")
create_test("vehicle" "${PROJECT_FILES}")

set(LUA_FILES
    ${PROJECT_FILES}

    ../src/GameScreen.h
    ../src/GameScreen.cpp

    ../src/Item.h
    ../src/Item.cpp

    ../src/ItemFactory.h
    ../src/ItemFactory.cpp

    ../src/Player.h
    ../src/Player.cpp

    ../src/Screen.h
    ../src/Screen.cpp

    ../src/TTLua.h
    ../src/TTLua.cpp

    ../src/Scenes/DebugWindow.h
    ../src/Scenes/DebugWindow.cpp

    ../src/Scenes/DescriptionText.h
    ../src/Scenes/DescriptionText.cpp

    ../src/Scenes/Hud.h
    ../src/Scenes/Hud.cpp

    ../src/Scenes/Opening.h
    ../src/Scenes/Opening.cpp

    ../src/Scenes/Scene.h
    ../src/Scenes/Scene.cpp
)

create_test("lua" "${LUA_FILES}")