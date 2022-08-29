add_library(db STATIC
    db_conn.cpp
    db_score.cpp
    db_song.cpp
)

target_include_directories(db PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(db PROPERTIES
    CXX_STANDARD 17)

target_link_libraries(db
    PUBLIC ${sqlite3_LIBRARY}
    PUBLIC common
    PUBLIC re2
)

target_precompile_headers(db REUSE_FROM common)
