
#include "utils.h"
#include <string>
#include <openssl/md5.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#endif

std::vector<fs::path> findFiles(fs::path p)
{
	auto pstr = p.u16string();
#ifdef _WIN32
	size_t offset = pstr.find(u"\\*");
#else
	size_t offset = pstr.find(u"/*");
#endif
	std::vector<fs::path> res;
	if (offset == pstr.npos)
	{
		if (!pstr.empty() && pstr.find(u'*') == pstr.npos)
			res.push_back(p);
		return res;
	}
	
	std::u16string dir = pstr.substr(0, offset);
	std::u16string tail = pstr.substr(offset + 2);
    if (fs::exists(dir))
        for (auto f : fs::directory_iterator(dir))
        {
            std::u16string file = f.path().u16string();
            if (file.substr(file.length() - tail.length()) != tail)
                continue;
            res.push_back(fs::canonical(f));
        }
	return res;
}

bool isParentPath(fs::path parent, fs::path dir)
{
    parent = fs::absolute(parent);
    dir = fs::absolute(dir);

    if (parent.root_directory() != dir.root_directory())
        return false;

    auto pair = std::mismatch(dir.begin(), dir.end(), parent.begin(), parent.end());
    return pair.second == parent.end();
}

int stoine(const std::string& str) noexcept
{
    try
    {
        // Not using atoi, since overflow behavior is undefined
        return std::stoi(str);
    }
    catch (std::invalid_argument&)
    {
        return 0;
    }
}

double stodne(const std::string& str) noexcept
{
    try
    {
        // Not using atoi, since overflow behavior is undefined
        return std::stod(str);
    }
    catch (std::invalid_argument&)
    {
        return 0;
    }
}

std::pair<unsigned, bool> stoub(const std::string& str)
{
    if (str.empty())
        return { -1, false };

    int val;
    bool notPref;
    if (str[0] == '!')
    {
        val = std::stoi(str.substr(1));
        notPref = true;
    }
    else
    {
        val = std::stoi(str);
        notPref = false;
    }

    if (val >= 0)
        return { val, notPref };
    else
        return { -1, false };
}
std::string md5(const std::string& str)
{
    return md5(str.c_str(), str.length());
}

std::string md5(const char* str, size_t len)
{
    auto digest = MD5((const unsigned char*)str, len, NULL);

    std::string ret;
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        unsigned char high = digest[i] >> 4 & 0xF;
        unsigned char low  = digest[i] & 0xF;
        ret += (high <= 9 ? ('0' + high) : ('A' - 10 + high));
        ret += (low  <= 9 ? ('0' + low)  : ('A' - 10 + low));
    }
    return ret;
}

std::string md5file(const fs::path& filePath)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    memset(digest, 0, sizeof(digest));
    if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
    {
        return "";
    }

    MD5_CTX mdContext;
    char data[1024];
    size_t bytes;

#ifdef _MSC_VER
    FILE* pf = NULL;
    fopen_s(&pf, filePath.string().c_str(), "rb");
#else
    FILE* pf = fopen(filePath.string().c_str(), "rb");
#endif
    if (pf == NULL) return "";

    MD5_Init(&mdContext);
    while ((bytes = fread(data, sizeof(char), sizeof(data), pf)) != 0)
        MD5_Update(&mdContext, data, bytes);
    MD5_Final(digest, &mdContext);
    fclose(pf);

    std::string ret;
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        unsigned char high = digest[i] >> 4 & 0xF;
        unsigned char low  = digest[i] & 0xF;
        ret += (high <= 9 ? ('0' + high) : ('A' - 10 + high));
        ret += (low  <= 9 ? ('0' + low)  : ('A' - 10 + low));
    }
    return ret;
}

std::string toLower(const std::string& s)
{
	std::string ret = s;
	for (auto& c : ret)
		if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 'a';
	return ret;
}

std::string toUpper(const std::string& s)
{
	std::string ret = s;
	for (auto& c : ret)
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';
	return ret;
}

bool is_shiftjis(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // ascii / hankaku gana
        if ((c <= 0x7f) || (c >= 0xa1 && c <= 0xdf))
            continue;

        // JIS X 0208
        else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xef))
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((c >= 0x40 && c <= 0x7e) || (c >= 0x80 && c <= 0xfc))
                continue;
        }

        // user defined
        else if (c >= 0xf0 && c <= 0xfc)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((c >= 0x40 && c <= 0x7e) || (c >= 0x80 && c <= 0xfc))
                continue;
        }

        else return false;
    }

    return true;
}

bool is_euckr(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // ascii
        if (c <= 0x7f) continue;

        // euc-jp
        if (c == 0x8e || c == 0x8f)
            return false;

        // gbk
        else if (0 /* nobody write bms in gbk */)
            return false;

        // shared range
        else if (c >= 0xa1 && c <= 0xfe)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if (c >= 0xa1 && c <= 0xfe)
                continue;
        }

        else return false;
    }

    return true;
}

bool is_utf8(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // invalid
        if ((c & 0b1100'0000) == 0b1000'0000 || (c & 0b1111'1110) == 0b1111'1110)
            return false;

        // 1 byte
        else if ((c & 0b1000'0000) == 0)
            continue;

        // 2~6 bytes
        else if ((c & 0b1110'0000) == 0b1100'0000) bytes = 2;
        else if ((c & 0b1111'0000) == 0b1110'0000) bytes = 3;
        else if ((c & 0b1111'1000) == 0b1111'0000) bytes = 4;
        else if ((c & 0b1111'1100) == 0b1111'1000) bytes = 5;
        else if ((c & 0b1111'1110) == 0b1111'1100) bytes = 6;
        else return false;

        while (--bytes)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((cc & 0b1100'0000) != 0b10000000) return false;
        }
    }

    return true;
}

eFileEncoding getFileEncoding(const fs::path& path)
{
    std::ifstream fs(path);
    if (fs.fail())
    {
        return eFileEncoding::LATIN1;
    }

    std::string buf;
    while (!fs.eof())
    {
        std::getline(fs, buf, '\n');

        if (is_shiftjis(buf)) return eFileEncoding::SHIFT_JIS;
        if (is_euckr(buf)) return eFileEncoding::EUC_KR;
        if (is_utf8(buf)) return eFileEncoding::UTF8;
    }

    return eFileEncoding::LATIN1;
}

std::string to_utf8(const std::string& input, eFileEncoding fromEncoding)
{
    switch (fromEncoding)
    {
    case eFileEncoding::SHIFT_JIS:
    {
#ifdef _WIN32
        DWORD dwNum;

        dwNum = MultiByteToWideChar(932, 0, input.c_str(), -1, NULL, 0);
        wchar_t* wstr = new wchar_t[dwNum];
        MultiByteToWideChar(932, 0, input.c_str(), -1, wstr, dwNum);

        dwNum = WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, NULL, 0, NULL, FALSE);
        char* ustr = new char[dwNum];
        WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, ustr, dwNum, NULL, FALSE);

        std::string ret(ustr);

        delete[] wstr;
        delete[] ustr;
        return ret;
#endif
    }

    case eFileEncoding::EUC_KR:
    {
#ifdef _WIN32
        DWORD dwNum;

        dwNum = MultiByteToWideChar(949, 0, input.c_str(), -1, NULL, 0);
        wchar_t* wstr = new wchar_t[dwNum];
        MultiByteToWideChar(949, 0, input.c_str(), -1, wstr, dwNum);

        dwNum = WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, NULL, 0, NULL, FALSE);
        char* ustr = new char[dwNum];
        WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, ustr, dwNum, NULL, FALSE);

        std::string ret(ustr);

        delete[] wstr;
        delete[] ustr;
        return ret;
#endif
    }

    case eFileEncoding::LATIN1:
    case eFileEncoding::UTF8:
    default:
        return input;
    }
}