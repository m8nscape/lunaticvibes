
#include "utils.h"
#include "types.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <charconv>

#ifdef WIN32
#include <Windows.h>
#include <wincrypt.h>
#else
#include <openssl/md5.h>
#endif



std::vector<Path> findFiles(Path p)
{
#ifdef WIN32
	auto pstr = p.u16string();
	size_t offset = pstr.find(u"\\*");
    if (offset == pstr.npos) offset = pstr.find(u"/*");
#else
	auto pstr = p.u8string();
	size_t offset = pstr.find(u"/*");
#endif

	std::vector<Path> res;
	if (offset == pstr.npos)
	{
		if (!pstr.empty() && pstr.find(u'*') == pstr.npos)
			res.push_back(p);
		return res;
	}
	
	auto dir = pstr.substr(0, offset);
	auto tail = pstr.substr(offset + 2);
    if (fs::exists(dir))
    {
        for (auto f : fs::directory_iterator(dir))
        {
#ifdef WIN32
            auto file = f.path().u16string();
#else
            auto file = f.path().u8string();
#endif
            if (file.substr(file.length() - tail.length()) != tail)
                continue;
            res.push_back(f.path());
        }
    }
	return res;
}

std::vector<Path> findFilesRecursive(Path p)
{
#ifdef WIN32
    auto pstr = p.u16string();
    size_t offset = pstr.find(u"\\*");
    if (offset == pstr.npos) offset = pstr.find(u"/*");
#else
    auto pstr = p.u8string();
    size_t offset = pstr.find(u"/*");
#endif

    std::vector<Path> res;
    if (offset == pstr.npos)
    {
        if (!pstr.empty() && pstr.find(u'*') == pstr.npos)
            res.push_back(p);
        return res;
    }

    auto dir = pstr.substr(0, offset);
    auto tail = pstr.substr(offset + 2);
    if (fs::exists(dir))
    {
        for (auto f : fs::recursive_directory_iterator(dir))
        {
#ifdef WIN32
            auto file = f.path().u16string();
#else
            auto file = f.path().u8string();
#endif
            if (file.substr(file.length() - tail.length()) != tail)
                continue;
            res.push_back(f.path());
        }
    }
    return res;
}

bool isParentPath(Path parent, Path dir)
{
    parent = fs::absolute(parent);
    dir = fs::absolute(dir);

    if (parent.root_directory() != dir.root_directory())
        return false;

    auto pair = std::mismatch(dir.begin(), dir.end(), parent.begin(), parent.end());
    return pair.second == parent.end();
}

// string to int
int toInt(std::string_view str, int defVal) noexcept
{
    int val = 0;
    if (auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), val); ec == std::errc())
        return val;
    else
        return defVal;
}
int toInt(const std::string& str, int defVal) noexcept { return toInt(std::string_view(str)); }

// string to double
double toDouble(std::string_view str, double defVal) noexcept
{
    double val = 0;
    if (auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), val); ec == std::errc())
        return val;
    else
        return defVal;
}
double toDouble(const std::string& str, double defVal) noexcept { return toDouble(std::string_view(str), defVal); }

// strcasecmp
bool strEqual(std::string_view str1, std::string_view str2, bool icase) noexcept
{
    if (icase)
    {
        return std::equal(std::execution::seq, str1.begin(), str1.end(), str2.begin(), str2.end(),
            [](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); });
    }
    else
    {
        return str1 == str2;
    }
}
bool strEqual(const std::string& str1, std::string_view str2, bool icase) noexcept { return strEqual(std::string_view(str1), str2, icase); }

std::string bin2hex(const void* bin, size_t size)
{
    std::string res;
    res.reserve(size * 2 + 1);
    constexpr char rgbDigits[] = "0123456789abcdef";
    for (size_t i = 0; i < size; ++i)
    {
        unsigned char c = ((unsigned char*)bin)[i];
        res += rgbDigits[(c >> 4) & 0xf];
        res += rgbDigits[(c >> 0) & 0xf];
    }
    return res;
}
std::string hex2bin(const std::string& hex)
{
    std::string res;
    res.resize(hex.length() / 2 + 1);
    for (size_t i = 0, j = 0; i < hex.length(); i += 2, j++)
    {
        unsigned char &c = ((unsigned char&)res[j]);
        char c1 = tolower(hex[i]);
        char c2 = tolower(hex[i + 1]);
        c += (c1 + ((c1 >= 'a') ? (10 - 'a') : (-'0'))) << 4;
        c += (c2 + ((c2 >= 'a') ? (10 - 'a') : (-'0')));
    }
    res[res.length() - 1] = '\0';
    return res;
}


HashMD5 md5(const std::string& str)
{
    return md5(str.c_str(), str.length());
}

#ifdef WIN32

HashMD5 md5(const char* str, size_t len)
{
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;

    CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

    CryptHashData(hHash, (const BYTE*)str, len, 0);

    constexpr size_t MD5_LEN = 16;
    BYTE rgbHash[MD5_LEN];
    DWORD cbHash = MD5_LEN;
    CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return bin2hex(rgbHash, MD5_LEN);
}

HashMD5 md5file(const Path& filePath)
{
    if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
    {
        return "";
    }

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;

    CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    while (!ifs.eof())
    {
        constexpr size_t BUFSIZE = 1024;
        char rgbFile[BUFSIZE];
        DWORD cbRead = 0;
        ifs.read(rgbFile, BUFSIZE);
        cbRead = ifs.gcount();
        if (cbRead == 0) break;
        CryptHashData(hHash, (const BYTE*)rgbFile, cbRead, 0);
    }

    constexpr size_t MD5_LEN = 16;
    BYTE rgbHash[MD5_LEN];
    DWORD cbHash = MD5_LEN;
    CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return bin2hex(rgbHash, MD5_LEN);
}

#else

HashMD5 md5(const char* str, size_t len)
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

HashMD5 md5file(const Path& filePath)
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
    fopen_s(&pf, filePath.u8string().c_str(), "rb");
#else
    FILE* pf = fopen(filePath.u8string().c_str(), "rb");
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

#endif

std::string toLower(std::string_view s)
{
	std::string ret(s);
	for (auto& c : ret)
		if (c >= 'A' && c <= 'Z')
			c = c - 'A' + 'a';
	return ret;
}
std::string toLower(const std::string& s) { return toLower(std::string_view(s)); }

std::string toUpper(std::string_view s)
{
	std::string ret(s);
	for (auto& c : ret)
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';
	return ret;
}
std::string toUpper(const std::string& s) { return toUpper(std::string_view(s)); }


Path convertLR2Path(const std::string& lr2path, const Path& relative_path)
{
    if (relative_path.is_absolute())
        return relative_path;

    return convertLR2Path(lr2path, relative_path.u8string());
}

std::string convertLR2Path(const std::string& lr2path, const std::string& relative_path)
{
    return convertLR2Path(lr2path, relative_path.c_str());
}

std::string convertLR2Path(const std::string& lr2path, const char* relative_path)
{
    if (auto p = Path(relative_path); p.is_absolute())
        return relative_path;

    std::string_view raw(relative_path);
    std::string_view prefix(relative_path, 2);
    if (!prefix.empty())
    {
        if (prefix == "./" || prefix == ".\\")
        {
            raw = raw.substr(2);
        }
        else if (prefix[0] == '/' || prefix[0] == '\\')
        {
            raw = raw.substr(1);
        }
    }
    prefix = raw.substr(0, 8);
    if (strEqual(prefix, "LR2files", true))
    {
        Path path(lr2path);
        path /= raw;
        return path.generic_string();
    }
    else
    {
        return relative_path;
    }
}