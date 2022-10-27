
find_package(Git)
if(GIT_EXECUTABLE)
    execute_process(COMMAND ${GIT_EXECUTABLE} branch --show-current
        OUTPUT_VARIABLE GIT_BRANCH)
    execute_process(COMMAND ${GIT_EXECUTABLE} log -n 1 --pretty=%h
        OUTPUT_VARIABLE GIT_COMMIT)

    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
    string(STRIP "${GIT_COMMIT}" GIT_COMMIT)
endif()

set (GIT_VERSION_HEADER "#pragma once
extern const char* GIT_BRANCH\;
extern const char* GIT_COMMIT\;
")
set (GIT_VERSION "#include \"git_version.h\"
const char* GIT_BRANCH = \"${GIT_BRANCH}\"\;
const char* GIT_COMMIT = \"${GIT_COMMIT}\"\;
")

if (EXISTS "git_version.h")
    file(STRINGS "git_version.h" GIT_VERSION_HEADER_ NEWLINE_CONSUME)
else()
    set(GIT_VERSION_HEADER_ "")
endif()
if (NOT "${GIT_VERSION_HEADER}" STREQUAL "${GIT_VERSION_HEADER_}")
    file(WRITE "git_version.h" ${GIT_VERSION_HEADER})
endif()

if (EXISTS "git_version.cpp")
    file(STRINGS "git_version.cpp" GIT_VERSION_ NEWLINE_CONSUME)
else()
    set(GIT_VERSION_ "")
endif()
if (NOT "${GIT_VERSION}" STREQUAL "${GIT_VERSION_}")
    file(WRITE "git_version.cpp" ${GIT_VERSION})
endif()