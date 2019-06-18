#include "utils.h"
#include <string>

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

int stoine(const std::string& str) noexcept
{
    try
    {
        // Not using atoi, since overflow behavior is undefined
        return std::stoi(str);
    }
    catch (std::invalid_argument& e)
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