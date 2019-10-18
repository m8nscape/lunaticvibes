#include "utils.h"
#include <string>
#include <openssl/md5.h>
#include <iostream>
#include <fstream>

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
        for (auto &f : fs::directory_iterator(dir))
        {
            std::u16string file = f.path().u16string();
            if (file.substr(file.length() - tail.length()) != tail)
                continue;
            res.push_back(f);
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

std::string md5(const fs::path& filePath)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    memset(digest, 0, sizeof(digest));
    if (fs::exists(filePath) && fs::is_regular_file(filePath))
    {
        MD5_CTX mdContext;
        char data[1024];
        size_t bytes;
        MD5_Init(&mdContext);
        std::ifstream ifs(filePath.string(), std::ios_base::binary);
        while ((bytes = size_t(ifs.readsome(data, 1024))) != 0)
            MD5_Update(&mdContext, data, bytes);
        MD5_Final(digest, &mdContext);
    }

    std::string ret(MD5_DIGEST_LENGTH * 2, 0);
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        unsigned char high = digest[i] >> 4 & 0xF;
        unsigned char low  = digest[i] & 0xF;
        ret += (high <= 9 ? ('0' + high) : ('A' - 10 + high));
        ret += (low  <= 9 ? ('0' + low)  : ('A' - 10 + low));
    }
    return ret;
}

std::string md5(const std::string& str)
{
    return md5(str.c_str(), str.length());
}

std::string md5(const char* str, size_t len)
{
    auto digest = MD5((const unsigned char*)str, len, NULL);

    std::string ret(MD5_DIGEST_LENGTH * 2, 0);
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        unsigned char high = digest[i] >> 4 & 0xF;
        unsigned char low  = digest[i] & 0xF;
        ret += (high <= 9 ? ('0' + high) : ('A' - 10 + high));
        ret += (low  <= 9 ? ('0' + low)  : ('A' - 10 + low));
    }
    return ret;
}