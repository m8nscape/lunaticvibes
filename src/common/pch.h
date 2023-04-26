#pragma once

// DO NOT ADD IMPLEMENTATIONS HERE

// std
#include <algorithm>
#include <any>
#include <array>
#include <bitset>
#include <cassert>
#include <charconv>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <deque>
#include <exception>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <random>
#include <shared_mutex>
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

// boost
#include <boost/algorithm/string.hpp>

#define BOOST_ASIO_NO_EXCEPTIONS
#include <boost/asio.hpp> 
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>


// 3rd-party
#include <re2/re2.h>
#include <yaml-cpp/yaml.h>
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>

// common
#include "types.h"
#include "fraction.h"
#include "meta.h"
#include "log.h"
#include "beat.h"
#include "utils.h"
#include "encoding.h"
#include "sysutil.h"
#include "hash.h"
#include "asynclooper.h"
#include "chartformat/chartformat.h"
#include "entry/entry.h"
