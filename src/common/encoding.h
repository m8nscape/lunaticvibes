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
std::u16string utf8_to_sjis(const std::string& str);
std::u32string utf8_to_utf32(const std::string& input);