find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
add_executable(setup WIN32)
else()
add_executable(setup)
endif()

target_sources(setup PUBLIC
    setup.cpp setupdialog.h setupdialog.cpp setupdialog.ui
)

set_target_properties(setup PROPERTIES
    CXX_STANDARD 17)

target_include_directories(setup PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${PROJECT_BINARY_DIR}/src/setup/setup_autogen/include
)

target_link_libraries(setup
    Qt5::Widgets
    common
    config
    fmod
)

if (${EXT_DEPEND_LIBFILES_SET})
    add_custom_command(TARGET setup POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_DEPEND_LIBFILES}
        $<TARGET_FILE_DIR:setup>
    )
endif()

add_custom_command(TARGET setup POST_BUILD
    COMMAND windeployqt $<TARGET_FILE_DIR:setup> $<TARGET_FILE:setup> --no-opengl --no-opengl-sw --translations en,zh_CN,zh_TW,ja,ko
)