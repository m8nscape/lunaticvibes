#pragma once
#include <filesystem>
#include <vector>
#include <utility>

namespace fs = std::filesystem;

inline fs::path executablePath;

// Following LR2skin path rules:
// Only filename (not including folders) could have wildcards "*"
// Searching is not recursive.
std::vector<fs::path> findFiles(fs::path path);

// For LR2 skin .csv parsing:
// op1~4 may include a '!' before the number, split it out
std::pair<unsigned, bool> stoub(const std::string&);
int stoine(const std::string& str) noexcept;

constexpr unsigned base36(char c)
{
	return (c > '9') ? (10 + c - 'A') : (c - '0');
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
	return (c > '9') ? (10 + c - 'A') : (c - '0');
}

constexpr unsigned base16(char first, char second)
{
	return 16 * base16(first) + base16(second);
}

constexpr unsigned base16(const char* c)
{
	return base16(c[0], c[1]);
}

std::string md5(fs::path filePath);