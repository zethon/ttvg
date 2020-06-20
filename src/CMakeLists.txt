set(SOURCE_FILES
    Engine.cpp
    GameScreen.cpp
    IntroScreen.cpp
    main.cpp
    ResourceManager.cpp
    Screen.cpp
    TTUtils.cpp
)

set(HEADER_FILES
    Engine.h
    GameScreen.h
    IntroScreen.h
    ResourceManager.h
    Screen.h
    TTUtils.h
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

# if(SFML_FOUND)
#     include_directories(${SFML_INCLUDE_DIR})
#     target_link_libraries(tooter PUBLIC ${SFML_DEPENDENCIES})
#     target_link_libraries(tooter PUBLIC ${SFML_LIBRARIES})
# endif()
