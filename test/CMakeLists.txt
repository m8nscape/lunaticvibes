enable_testing()
include(GoogleTest)

add_executable(apptest 
    test_main.cpp
    test_config.cpp
    test_db.cpp
    common/test_fraction.cpp
    common/test_chartformat_bms.cpp
    game/test_graphics.cpp
)
target_link_libraries(apptest PUBLIC
    GTest::gtest GTest::gmock)

#Disable CTest for now. Sprite tests require loading texture at main thread, which CTest appears not to provide.
#Run test app directly Until I found a solution...
#gtest_discover_tests(apptest)

set_target_properties(apptest PROPERTIES
    CXX_STANDARD 17
    CTEST_OUTPUT_ON_FAILURE TRUE 
    GTEST_COLOR TRUE
)
    
target_link_libraries(apptest 
    PUBLIC gamelib
    PUBLIC plog
)
target_include_directories(apptest PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

if (${EXT_DEPEND_LIBFILES_SET})
    add_custom_command(TARGET apptest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_DEPEND_LIBFILES}
        $<TARGET_FILE_DIR:apptest>
    )
    add_custom_command(TARGET apptest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/test
        $<TARGET_FILE_DIR:apptest>/test
    )
endif()
