add_library(common STATIC
    asynclooper.cpp
    encoding.cpp
    fraction.cpp
    log.cpp
    utils.cpp
    sysutil_win.cpp
    sysutil_linux.cpp
    chartformat/chartformat.cpp
    chartformat/format_bms.cpp
    entry/entry_folder.cpp
)

target_include_directories(common PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(common PROPERTIES
    CXX_STANDARD 17)

target_link_libraries(common 
    PUBLIC plog
)

target_precompile_headers(common PUBLIC pch.h)