add_library(config STATIC
    config.cpp
    config_mgr.cpp
    cfg_general.cpp
    cfg_input.cpp
    cfg_profile.cpp
    cfg_skin.cpp
)

target_include_directories(config PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(config PROPERTIES
    CXX_STANDARD 17)

target_link_libraries(config 
    PUBLIC yaml-cpp
    PUBLIC common
)

target_precompile_headers(config REUSE_FROM common)