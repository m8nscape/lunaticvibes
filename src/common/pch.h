#pragma once

// DO NOT ADD IMPLEMENTATIONS HERE

// common
#include "asynclooper.h"
#include "beat.h"
#include "encoding.h"
#include "fraction.h"
#include "log.h"
#include "meta.h"
#include "sysutil.h"
#include "types.h"
#include "hash.h"
#include "utils.h"
#include "chartformat/chartformat.h"
#include "entry/entry.h"

// std
#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <exception>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

// system
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif