#pragma once
#include "cmake_config.h"

#define PROJECT_SUB_NAME "Alpha"
#define PROJECT_FULL_NAME (PROJECT_NAME " " PROJECT_SUB_NAME)

constexpr char BUILD_TIME[] = __TIMESTAMP__;

constexpr unsigned CANVAS_WIDTH = 1280;
constexpr unsigned CANVAS_HEIGHT = 720;