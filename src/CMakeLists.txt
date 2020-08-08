set(SOURCE_FILES
    AnimatedSprite.cpp
    Background.cpp
    Engine.cpp
    GameScreen.cpp
    Intersection.cpp
    IntroScreen.cpp
    main.cpp
    PathFactory.cpp
    Player.cpp
    ResourceManager.cpp
    Screen.cpp
    TransitionPoint.cpp
    TTUtils.cpp
    Vehicle.cpp
    VehicleFactory.cpp

    Item.cpp
    ItemFactory.cpp

    Scenes/DescriptionText.cpp
    Scenes/Hud.cpp
    Scenes/EuclidHouse.cpp
    Scenes/Opening.cpp
    Scenes/Scene.cpp
)

set(HEADER_FILES
    AnimatedSprite.h
    Background.h
    Engine.h
    GameScreen.h
    GameTypes.h
    Intersection.h
    IntroScreen.h
    Path.hpp
    PathFactory.h
    Player.h
    ResourceManager.h
    Screen.h
    Tiles.hpp
    TransitionPoint.h
    TTUtils.h
    Vehicle.h
    VehicleFactory.h
    
    Item.h
    ItemFactory.h

    Scenes/DescriptionText.h
    Scenes/Hud.h
    Scenes/EuclidHouse.h
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

