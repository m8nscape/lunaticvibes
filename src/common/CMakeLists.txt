add_library(common STATIC
    asynclooper.cpp
    encoding.cpp
    fraction.cpp
    log.cpp
    utils.cpp
    keymap.cpp
    sysutil.cpp
    sysutil_win.cpp
    sysutil_linux.cpp
    chartformat/chartformat.cpp
    chartformat/chartformat_bms.cpp
    entry/entry_folder.cpp
    entry/entry_song.cpp
    difficultytable/difficultytable.cpp
    difficultytable/table_bms.cpp
    coursefile/lr2crs.cpp)

target_include_directories(common PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(common PROPERTIES
    CXX_STANDARD 17)
    
target_link_libraries(common 
    PUBLIC plog
    PUBLIC re2
    PUBLIC taocpp::json
)

# Add OpenSSL (from system)
if (UNIX OR MINGW)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(common PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

target_precompile_headers(common PUBLIC pch.h)