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
elseif (LINUX)

endif()

target_include_directories(SQLite3 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

get_target_property(SQLITE3_LIB_FILES SQLite3 IMPORTED_LOCATION)
set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES} 
        ${SQLITE3_LIB_FILES}
        CACHE INTERNAL "dependencies"
)
set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
