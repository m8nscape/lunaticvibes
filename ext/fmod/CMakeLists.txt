add_library (fmod SHARED IMPORTED)

if (${BUILD_X64})
    set(FMOD_SUFFIX_64 64)
endif()

if (BUILD_DEBUG)
    set(FMOD_SUFFIX_L L)
endif()

if (WIN32)
    set_target_properties(fmod PROPERTIES
        IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/fmod${FMOD_SUFFIX_L}${FMOD_SUFFIX_64}.dll
        IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/fmod${FMOD_SUFFIX_L}${FMOD_SUFFIX_64}_vc.lib
    )
elseif (LINUX)

endif()

target_include_directories(fmod INTERFACE ${CMAKE_CURRENT_LIST_DIR}/inc)