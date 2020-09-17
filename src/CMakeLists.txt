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

message("IH HITER!!")

install(
    TARGETS tooter 
    COMPONENT tooter
    RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	BUNDLE DESTINATION .
)

message("LIB: ${CMAKE_CURRENT_SOURCE_DIR}/resources/${RES_FOLDER}")

set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
include(InstallRequiredSystemLibraries)
INSTALL(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION bin
        COMPONENT tooter)

# foreach(RES_FOLDER fonts images)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
        DESTINATION bin/resources
        COMPONENT tooter
        PATTERN "src" EXCLUDE
        )
# install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
#         DESTINATION ${LIBRARY_OUTPUT_PATH}
#         FILES_MATCHING PATTERN "*_rdict.pcm"
# )
message("LIB: ${CMAKE_CURRENT_SOURCE_DIR}/resources/${RES_FOLDER}")
# message("$<TARGET_FILE_DIR:tooter>")

# endforeach(RES_FOLDER )


# install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
#         DESTINATION ${LIBRARY_OUTPUT_PATH}
#         FILES_MATCHING PATTERN "*_rdict.pcm"
# )

foreach(_CONFIG_FILE_ TTVG.CPackConfig.cmake TTVGPackage.cmake)
    configure_file("CPack/${_CONFIG_FILE_}.in" "${CMAKE_BINARY_DIR}/${_CONFIG_FILE_}" @ONLY)
endforeach()

set(CPACK_PACKAGE_CONTACT "Support <max@amb.dog>")

include(CPack)

