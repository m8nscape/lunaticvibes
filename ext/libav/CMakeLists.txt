if (${BUILD_X64})
    set(LIBAV_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/x64)
else()
    set(LIBAV_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/x86)
endif()

if (WIN32)
    set(LIBAV_LIB_FILES 
        ${LIBAV_LIB_PATH}/avcodec-57.dll
        ${LIBAV_LIB_PATH}/avdevice-56.dll
        ${LIBAV_LIB_PATH}/avfilter-6.dll
        ${LIBAV_LIB_PATH}/avformat-57.dll
        ${LIBAV_LIB_PATH}/avresample3.dll
        ${LIBAV_LIB_PATH}/avutil-55.dll
        ${LIBAV_LIB_PATH}/swscale-4.dll
        ${LIBAV_LIB_PATH}/libwinpthread-1.dll
        )
    set(LIBAV_IMPORT_LIBS
        ${LIBAV_LIB_PATH}/avcodec.lib
        ${LIBAV_LIB_PATH}/avdevice.lib
        ${LIBAV_LIB_PATH}/avfilter.lib
        ${LIBAV_LIB_PATH}/avformat.lib
        ${LIBAV_LIB_PATH}/avresample.lib
        ${LIBAV_LIB_PATH}/avutil.lib
        ${LIBAV_LIB_PATH}/swscale.lib
        )
else ()
    set(LIBAV_LIB_FILES )
    set(LIBAV_IMPORT_LIBS )
endif()

add_library(libavcodec    SHARED IMPORTED GLOBAL)
add_library(libavdevice   SHARED IMPORTED GLOBAL)
add_library(libavfilter   SHARED IMPORTED GLOBAL)
add_library(libavformat   SHARED IMPORTED GLOBAL)
add_library(libavresample SHARED IMPORTED GLOBAL)
add_library(libavutil     SHARED IMPORTED GLOBAL)
add_library(libswscale    SHARED IMPORTED GLOBAL)
target_include_directories(libavcodec    INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libavdevice   INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libavfilter   INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libavformat   INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libavresample INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libavutil     INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libswscale    INTERFACE ${CMAKE_CURRENT_LIST_DIR})
    
if (WIN32)
    set_target_properties(libavcodec PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avcodec-57.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avcodec.lib
    )
    set_target_properties(libavdevice PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avdevice-56.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avdevice.lib
    )
    set_target_properties(libavfilter PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avfilter-6.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avfilter.lib
    )
    set_target_properties(libavformat PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avformat-57.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avformat.lib
    )
    set_target_properties(libavresample PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avresample3.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avresample.lib
    )
    set_target_properties(libavutil PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/avutil-55.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/avutil.lib
    )
    set_target_properties(libswscale PROPERTIES
        IMPORTED_LOCATION   ${LIBAV_LIB_PATH}/swscale-4.dll
        IMPORTED_IMPLIB     ${LIBAV_LIB_PATH}/swscale.lib
    )
elseif (LINUX)

endif()

set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES} 
        ${LIBAV_LIB_FILES}
        CACHE INTERNAL "dependencies"
)
set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
