#pragma once
#include "cmake_config.h"

namespace lunaticvibes
{

#define PROJECT_SUB_NAME "Alpha"
#define PROJECT_FULL_NAME (PROJECT_NAME " " PROJECT_SUB_NAME)

constexpr char BUILD_TIME[] = __TIMESTAMP__;

#define GAMEDATA_PATH "gamedata"

constexpr unsigned CANVAS_WIDTH = 1280;
constexpr unsigned CANVAS_HEIGHT = 720;

constexpr unsigned CANVAS_WIDTH_MAX = 1920 * 3;
constexpr unsigned CANVAS_HEIGHT_MAX = 1080 * 3;

}
