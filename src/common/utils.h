#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <vector>
#include <utility>
#include <memory>

#include "hash.h"
#include "types.h"

namespace fs = std::filesystem;

inline Path executablePath;

// path may include wildcard (*, ?)
std::vector<Path> findFiles(Path path, bool recursive = false);

bool isParentPath(Path parent, Path dir);

// string to int
int toInt(const std::string& str, int defVal = 0) noexcept;
int toInt(std::string_view str, int defVal = 0) noexcept;
// string to double
double toDouble(const std::string& str, double defVal = 0.0) noexcept;
double toDouble(std::string_view str, double defVal = 0.0) noexcept;
// strcasecmp
bool strEqual(const std::string& str1, std::string_view str2, bool icase = false) noexcept;
bool strEqual(std::string_view str1, std::string_view str2, bool icase = false) noexcept;

constexpr unsigned base36(char c)
{
	return (c > '9') ? (c >= 'a' ? 10 + c - 'a' : 10 + c - 'A') : (c - '0');
}

constexpr unsigned base36(char first, char second)
{
	return 36 * base36(first) + base36(second);
}

constexpr unsigned base36(const char* c)
{
	return base36(c[0], c[1]);
}

constexpr unsigned base16(char c)
{
	return (c > '9') ? (c >= 'a' ? 10 + c - 'a' : 10 + c - 'A') : (c - '0');
}

constexpr unsigned base16(char first, char second)
{
	return 16 * base16(first) + base16(second);
}

constexpr unsigned base16(const char* c)
{
	return base16(c[0], c[1]);
}

std::string bin2hex(const void* bin, size_t size);
std::string hex2bin(const std::string& hex);

std::string toLower(std::string_view s);
std::string toLower(const std::string& s);
std::string toUpper(std::string_view s);
std::string toUpper(const std::string& s);

std::string convertLR2Path(const std::string& lr2path, const Path& relative_path);
std::string convertLR2Path(const std::string& lr2path, const std::string& relative_path_utf8);
std::string convertLR2Path(const std::string& lr2path, const char* relative_path_utf8);
std::string convertLR2Path(const std::string& lr2path, std::string_view relative_path_utf8);

Path PathFromUTF8(std::string_view s);

void preciseSleep(long long nanoseconds);

double normalizeLinearGrowth(double prev, double curr);
