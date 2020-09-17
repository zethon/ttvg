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

if(WIN32)
    set(GUI_TYPE WIN32)
endif()

if(APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
endif()

add_executable(tooter ${GUI_TYPE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_link_libraries(tooter
PUBLIC
    ${CONAN_LIBS}
    ${SFML_Libraries}
    ${CONAN_LIBS_FRAMEWORKS_LUADIST}
)

if(WIN32)
    set_target_properties(tooter PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(tooter PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
    set_target_properties(tooter PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    set_target_properties(tooter PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
endif()

install(
    TARGETS tooter 
    COMPONENT tooter
    RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	BUNDLE DESTINATION .
)
set(CMAKE_INSTALL_UCRT_LIBRARIES FALSE)
include(InstallRequiredSystemLibraries)
install(
    PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION bin
    COMPONENT tooter
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
    DESTINATION bin/resources
    COMPONENT tooter
    PATTERN "src" EXCLUDE
)

foreach(_CONFIG_FILE_ TTVGCPackConfig.cmake TTVGPackage.cmake)
    configure_file("CPack/${_CONFIG_FILE_}.in" "${CMAKE_BINARY_DIR}/${_CONFIG_FILE_}" @ONLY)
endforeach()

set(CPACK_PACKAGE_CONTACT "Support <max@amb.dog>")

include(CPack)

