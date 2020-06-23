set(SOURCE_FILES
    AnimatedSprite.cpp
    Engine.cpp
    ExitScreen.cpp
    GameScreen.cpp
    IntroScreen.cpp
    main.cpp
    ResourceManager.cpp
    Screen.cpp
    TTUtils.cpp

    Scenes/Opening.cpp
)

set(HEADER_FILES
    AnimatedSprite.h
    Engine.h
    ExitScreen.h
    GameScreen.h
    IntroScreen.h
    ResourceManager.h
    Screen.h
    TTUtils.h

    Scenes/Opening.h
)

add_executable(tooter
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

message(INFO "tt: ${CONAN_LIBS_BOOST}")

target_link_libraries(tooter
PUBLIC
    ${CONAN_LIBS}
    ${SFML_Libraries}
)

