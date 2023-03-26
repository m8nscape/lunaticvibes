add_library(SQLite3 SHARED IMPORTED GLOBAL)
set(USE_BUNDLED_SQLITE3 TRUE CACHE BOOL "USE_BUNDLED_SQLITE3")

if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SQLite3 PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/sqlite3.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/sqlite3.lib  
        )
    else()
        set_target_properties(SQLite3 PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/sqlite3.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/sqlite3.lib  
        )
    endif()
else()
    message(FATAL_ERROR "There is no bundled SQLite3 for this platform")
endif()

target_include_directories(SQLite3 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
