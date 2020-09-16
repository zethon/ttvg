set(SOURCE_FILES
    AnimatedSprite.cpp
    Background.cpp
    Engine.cpp
    GameOverScreen.cpp
    GameScreen.cpp
    Intersection.cpp
    IntroScreen.cpp
    Item.cpp
    ItemFactory.cpp
    main.cpp
    PathFactory.cpp
    Player.cpp
    ResourceManager.cpp
    Screen.cpp
    TooterLogger.cpp
    Transition.cpp
    TTLua.cpp
    TTUtils.cpp
    Vehicle.cpp
    VehicleFactory.cpp
    Zone.cpp

    Scenes/DebugWindow.cpp
    Scenes/DescriptionText.cpp
    Scenes/Hud.cpp
    Scenes/ModalWindow.cpp
    Scenes/Tucson.cpp
    Scenes/Scene.cpp
)

set(HEADER_FILES
    AnimatedSprite.h
    Background.h
    core.h
    Engine.h
    GameOverScreen.h
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
    TooterLogger.h
    Transition.h
    TTLua.h
    TTUtils.h
    Vehicle.h
    VehicleFactory.h
    Zone.h
    
    Item.h
    ItemFactory.h

    Scenes/DebugWindow.h
    Scenes/DescriptionText.h
    Scenes/Hud.h
    Scenes/ModalWindow.h
    Scenes/Tucson.h
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
    ${CONAN_LIBS_FRAMEWORKS_LUADIST}
)

