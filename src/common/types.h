#pragma once
#include <filesystem>
#include <utility>
#include <string>

typedef std::filesystem::path Path;
typedef decltype(std::declval<Path>().string()) StringPath;
typedef std::string StringContent; // std::ifstream, std::getline
