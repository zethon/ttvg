set(SOURCE_FILES
    AnimatedSprite.cpp
    Background.cpp
    Engine.cpp
    ExitScreen.cpp
    GameScreen.cpp
    Intersection.cpp
    IntroScreen.cpp
    main.cpp
    PathFactory.cpp
    Player.cpp
    ResourceManager.cpp
    Screen.cpp
    TTUtils.cpp
    Vehicle.cpp
    VehicleFactory.cpp

    Scenes/Opening.cpp
    Scenes/Scene.cpp
)

set(HEADER_FILES
    AnimatedSprite.h
    Background.h
    Engine.h
    ExitScreen.h
    GameScreen.h
    Intersection.h
    IntroScreen.h
    Path.hpp
    PathFactory.h
    Player.h
    ResourceManager.h
    Screen.h
    Tiles.hpp
    TTUtils.h
    Vehicle.h
    VehicleFactory.h
    
    Scenes/Opening.h
    Scenes/Scene.h
)

add_executable(tooter
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_link_libraries(tooter
PUBLIC
    ${CONAN_LIBS}
    ${SFML_Libraries}
)

