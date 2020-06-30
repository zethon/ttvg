set(SOURCE_FILES
    AnimatedSprite.cpp
    Background.cpp
    Engine.cpp
    ExitScreen.cpp
    GameScreen.cpp
    IntroScreen.cpp
    main.cpp
    Player.cpp
    ResourceManager.cpp
    Screen.cpp
    TTUtils.cpp

    Scenes/Opening.cpp
    Scenes/Scene.cpp
)

set(HEADER_FILES
    AnimatedSprite.h
    Background.h
    Engine.h
    ExitScreen.h
    GameScreen.h
    IntroScreen.h
    Player.h
    ResourceManager.h
    Screen.h
    TTUtils.h
    
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

