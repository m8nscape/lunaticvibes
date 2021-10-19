find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(setup
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
