add_library (fmod SHARED IMPORTED GLOBAL)

if (BUILD_DEBUG)
    set(FMOD_SUFFIX_L L)
endif()

if (WIN32)
    if (${BUILD_X64})
        set_target_properties(fmod PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/fmod${FMOD_SUFFIX_L}.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/fmod${FMOD_SUFFIX_L}_vc.lib  
        )
    else()
        set_target_properties(fmod PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/fmod${FMOD_SUFFIX_L}.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/fmod${FMOD_SUFFIX_L}_vc.lib  
        )
    endif()
elseif (LINUX)

endif()

target_include_directories(fmod INTERFACE ${CMAKE_CURRENT_LIST_DIR}/inc)

get_target_property(FMOD_LIB_FILES fmod IMPORTED_LOCATION)
set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES} 
        ${FMOD_LIB_FILES}
        CACHE INTERNAL "dependencies"
)
set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
