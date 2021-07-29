
#include "utils.h"
#include "types.h"
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>

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
#else
	auto pstr = p.string();
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
        for (auto f : fs::directory_iterator(dir))
        {
#ifdef WIN32
            auto file = f.path().u16string();
#else
            auto file = f.path().string();
#endif
            if (file.substr(file.length() - tail.length()) != tail)
                continue;
            res.push_back(fs::canonical(f));
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

std::string bin2hex(const void* bin, size_t size)
{
    std::string res;
    res.reserve(size * 2 + 1);
    static const char rgbDigits[] = "0123456789abcdef";
    for (size_t i = 0; i < size; ++i)
    {
        unsigned char c = ((unsigned char*)bin)[i];
        res += rgbDigits[(c >> 4) & 0xf];
        res += rgbDigits[(c >> 0) & 0xf];
    }
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

    static const size_t MD5_LEN = 16;
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
        static const size_t BUFSIZE = 1024;
        char rgbFile[BUFSIZE];
        DWORD cbRead = 0;
        ifs.read(rgbFile, BUFSIZE);
        cbRead = ifs.gcount();
        if (cbRead == 0) break;
        CryptHashData(hHash, (const BYTE*)rgbFile, cbRead, 0);
    }

    static const size_t MD5_LEN = 16;
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

#endif

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
