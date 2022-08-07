#pragma once
#include "types.h"

enum class eFileEncoding
{
	LATIN1,
	SHIFT_JIS,
	EUC_KR,
	UTF8,
};
eFileEncoding getFileEncoding(const Path& path);
eFileEncoding getFileEncoding(std::istream& is);
const char* getFileEncodingName(eFileEncoding enc);

std::string to_utf8(const std::string& str, eFileEncoding fromEncoding);
std::string from_utf8(const std::string& input, eFileEncoding toEncoding);
std::u32string to_utf32(const std::string& str, eFileEncoding fromEncoding);
std::string from_utf32(const std::u32string& input, eFileEncoding toEncoding);

std::u32string utf8_to_utf32(const std::string& input);
std::string utf32_to_utf8(const std::u32string& str);

