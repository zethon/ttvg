set(SOURCE_FILES
    main.cpp
    TTUtils.cpp
)

set(HEADER_FILES
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
