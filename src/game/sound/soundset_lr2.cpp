#include "soundset_lr2.h"
#include "common/encoding.h"
#include "config/config_mgr.h"
#include "game/scene/scene_customize.h"

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

SoundSetLR2::SoundSetLR2()
{
	_type = eSoundSetType::LR2;
}

SoundSetLR2::SoundSetLR2(Path p) : SoundSetLR2()
{
	loadCSV(p);
}

void SoundSetLR2::loadCSV(Path p)
{
	if (filePath.empty())
		filePath = p;

    auto srcLineNumberParent = csvLineNumber;
    csvLineNumber = 0;

    p = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), p));

    std::ifstream ifsFile(p, std::ios::binary);
    if (!ifsFile.is_open())
    {
        LOG_ERROR << "[SoundSet] File Not Found: " << std::filesystem::absolute(p).u8string();
        csvLineNumber = srcLineNumberParent;
        return;
    }

    // copy the whole file into ram, once for all
    std::stringstream csvFile;
    csvFile << ifsFile.rdbuf();
    csvFile.sync();
    ifsFile.close();

    auto encoding = getFileEncoding(csvFile);

    LOG_INFO << "[SoundSet] File (" << getFileEncodingName(encoding) << "): " << p.u8string();

    std::vector<StringContent> tokenBuf;
    tokenBuf.reserve(32);

    while (!csvFile.eof())
    {
        std::string raw;
        std::getline(csvFile, raw);
        ++csvLineNumber;

        // convert codepage
        std::string rawUTF8 = to_utf8(raw, encoding);

        static boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char>> tokens(raw, sep);
        if (tokens.begin() == tokens.end()) continue;
        tokenBuf.assign(tokens.begin(), tokens.end());

        parseHeader(tokenBuf);
    }

    // load skin customization from profile
    Path pCustomize = ConfigMgr::Profile()->getPath() / "customize" / SceneCustomize::getConfigFileName(getFilePath());
    try
    {
        std::map<StringContent, StringContent> opFileMap;
        for (const auto& node : YAML::LoadFile(pCustomize.u8string()))
        {
            auto key = node.first.as<std::string>();
            if (key.substr(0, 5) == "FILE_")
            {
                opFileMap[key.substr(5)] = node.second.as<std::string>();
            }
        }
        for (auto& itOp : customfiles)
        {
            for (auto& itFile : opFileMap)
            {
                if (itOp.title == itFile.first)
                {
                    if (const auto itEntry = std::find(itOp.label.begin(), itOp.label.end(), itFile.second); itEntry != itOp.label.end())
                    {
                        itOp.value = std::distance(itOp.label.begin(), itEntry);
                    }
                }
            }
        }
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Skin] Bad customize config file: " << pCustomize.u8string();
    }

    csvFile.clear();
    csvFile.seekg(0);
    csvLineNumber = 0;
    while (!csvFile.eof())
    {
        std::string raw;
        std::getline(csvFile, raw);
        ++csvLineNumber;

        // convert codepage
        std::string rawUTF8 = to_utf8(raw, encoding);

        // remove not needed spaces
#ifdef WIN32
        static const auto localeUTF8 = std::locale(".65001");
#else
        static const auto localeUTF8 = std::locale("en_US.UTF-8");
#endif
        boost::trim_right(rawUTF8, localeUTF8);

        static boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char>> tokens(rawUTF8, sep);
        if (tokens.begin() == tokens.end()) continue;
        tokenBuf.assign(tokens.begin(), tokens.end());

        parseBody(tokenBuf);
    }

    LOG_DEBUG << "[SoundSet] File: " << p.u8string() << "(Line " << csvLineNumber << "): loading finished";
}

bool SoundSetLR2::parseHeader(const std::vector<StringContent>& tokens)
{
    StringContentView key = tokens[0];

    if (strEqual(key, "#INFORMATION", true))
    {
        if (tokens.size() > 2) name = tokens[2];
        if (tokens.size() > 3) maker = tokens[3];
        if (tokens.size() > 4) thumbnailPath = tokens[4];

        return true;
    }
    else if (strEqual(key, "#CUSTOMFILE", true))
    {
        auto& title(tokens[1]);
        auto& path(tokens[2]);
        Path pathf = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), path));
        auto& def(tokens[3]);

        auto ls = findFiles(pathf);
        size_t defVal = 0;
        for (size_t param = 0; param < ls.size(); ++param)
        {
            if (ls[param].filename().stem().u8string() == def)
            {
                defVal = param;
                break;
            }
        }

        LOG_DEBUG << "[SoundSet] " << csvLineNumber << ": Loaded Custom file " << title << ": " << pathf.u8string();

        CustomFile c;
        c.title = title;
        c.filepath = pathf.u8string();
        for (auto& p : ls)
        {
            c.label.push_back(p.filename().stem().u8string());
            c.pathList.push_back(p);
        }
        c.label.push_back("RANDOM");
        c.pathList.push_back("RANDOM");
        c.defIdx = defVal;
        c.value = defVal;
        customfiles.push_back(c);

        std::srand(std::time(NULL));
        customizeRandom.push_back(ls.empty() ? 0 : (std::rand() % ls.size()));

        return true;
    }
    return false;
}
bool SoundSetLR2::parseBody(const std::vector<StringContent>& tokens)
{
    StringContentView key = tokens[0];

    static const std::set<std::string> soundKeys =
    {
        "#SELECT",
        "#DECIDE",
        "#EXSELECT",
        "#EXDECIDE",
        "#FOLDER_OPEN",
        "#FOLDER_CLOSE",
        "#PANEL_OPEN",
        "#PANEL_CLOSE",
        "#OPTION_CHANGE",
        "#DIFFICULTY",
        "#SCREENSHOT",
        "#CLEAR",
        "#FAIL",
        "#STOP",
        "#MINE",
        "#SCRATCH",
        "#COURSECLEAR",
        "#COURSEFAIL",
    };
    for (auto& k : soundKeys)
    {
        if (strEqual(k, key, true))
        {
            loadPath(k, tokens[1]);

            return true;
        }
    }
    return false;
}


bool SoundSetLR2::loadPath(const std::string& key, const std::string& rawpath)
{
    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), rawpath));
    StringPath pathStr = path.native();
    std::string pathU8Str = path.u8string();
    if (pathStr.find("*"_p) != pathStr.npos)
    {
        bool customFileFound = false;

        // Check if the wildcard path is specified by custom settings
        std::srand(std::time(NULL));
        for (size_t idx = 0; idx < customfiles.size(); ++idx)
        {
            const auto& cf = customfiles[idx];
            if (cf.filepath == pathU8Str.substr(0, cf.filepath.length()))
            {
                int value = (cf.pathList[cf.value] == "RANDOM") ? customizeRandom[idx] : cf.value;

                Path pathFile = cf.pathList[value];
                if (cf.filepath.length() < pathU8Str.length())
                    pathFile /= PathFromUTF8(pathU8Str.substr(cf.filepath.length() + 1));

                soundFilePath[key] = pathFile;
                LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added " << key << ": " << pathFile.u8string();

                customFileFound = true;
                break;
            }
        }

        if (!customFileFound)
        {
            // Or, randomly choose a file
            auto ls = findFiles(path);
            if (ls.empty())
            {
                soundFilePath[key] = Path();
                LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added random " << key << ": " << "(placeholder)";
            }
            else
            {
                size_t ranidx = std::rand() % ls.size();
                Path soundPath = PathFromUTF8(ls[ranidx].u8string());
                soundFilePath[key] = soundPath;
                LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added random " << key << ": " << soundPath.u8string();
            }
        }
    }
    else
    {
        // Normal path
        soundFilePath[key] = path;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added " << key << ": " << path.u8string();
    }

    return soundFilePath.find(key) != soundFilePath.end() && !soundFilePath[key].empty();
}


Path SoundSetLR2::getPathBGMSelect() const
{
    return soundFilePath.find("#SELECT") != soundFilePath.end() ? soundFilePath.at("#SELECT") : Path();
}

Path SoundSetLR2::getPathBGMDecide() const
{
    return soundFilePath.find("#DECIDE") != soundFilePath.end() ? soundFilePath.at("#DECIDE") : Path();
}

Path SoundSetLR2::getPathSoundOpenFolder() const
{
    return soundFilePath.find("#FOLDER_OPEN") != soundFilePath.end() ? soundFilePath.at("#FOLDER_OPEN") : Path();
}

Path SoundSetLR2::getPathSoundCloseFolder() const
{
    return soundFilePath.find("#FOLDER_CLOSE") != soundFilePath.end() ? soundFilePath.at("#FOLDER_CLOSE") : Path();
}

Path SoundSetLR2::getPathSoundOpenPanel() const
{
    return soundFilePath.find("#PANEL_OPEN") != soundFilePath.end() ? soundFilePath.at("#PANEL_OPEN") : Path();
}

Path SoundSetLR2::getPathSoundClosePanel() const
{
    return soundFilePath.find("#PANEL_CLOSE") != soundFilePath.end() ? soundFilePath.at("#PANEL_CLOSE") : Path();
}

Path SoundSetLR2::getPathSoundOptionChange() const
{
    return soundFilePath.find("#OPTION_CHANGE") != soundFilePath.end() ? soundFilePath.at("#OPTION_CHANGE") : Path();
}

Path SoundSetLR2::getPathSoundDifficultyChange() const
{
    return soundFilePath.find("#DIFFICULTY") != soundFilePath.end() ? soundFilePath.at("#DIFFICULTY") : Path();
}

Path SoundSetLR2::getPathSoundScreenshot() const
{
    return soundFilePath.find("#SCREENSHOT") != soundFilePath.end() ? soundFilePath.at("#SCREENSHOT") : Path();
}

Path SoundSetLR2::getPathBGMResultClear() const
{
    return soundFilePath.find("#CLEAR") != soundFilePath.end() ? soundFilePath.at("#CLEAR") : Path();
}

Path SoundSetLR2::getPathBGMResultFailed() const
{
    return soundFilePath.find("#FAIL") != soundFilePath.end() ? soundFilePath.at("#FAIL") : Path();
}

Path SoundSetLR2::getPathSoundFailed() const
{
    return soundFilePath.find("#STOP") != soundFilePath.end() ? soundFilePath.at("#STOP") : Path();
}

Path SoundSetLR2::getPathSoundLandmine() const
{
    return soundFilePath.find("#MINE") != soundFilePath.end() ? soundFilePath.at("#MINE") : Path();
}

Path SoundSetLR2::getPathSoundScratch() const
{
    return soundFilePath.find("#SCRATCH") != soundFilePath.end() ? soundFilePath.at("#SCRATCH") : Path();
}

Path SoundSetLR2::getPathBGMCourseClear() const
{
    return soundFilePath.find("#COURSECLEAR") != soundFilePath.end() ? soundFilePath.at("#COURSECLEAR") : Path();
}

Path SoundSetLR2::getPathBGMCourseFailed() const
{
    return soundFilePath.find("#COURSEFAIL") != soundFilePath.end() ? soundFilePath.at("#COURSEFAIL") : Path();
}


size_t SoundSetLR2::getCustomizeOptionCount() const
{
    return customfiles.size();
}

vSkin::CustomizeOption SoundSetLR2::getCustomizeOptionInfo(size_t idx) const
{
    vSkin::CustomizeOption ret;
    const auto& op = customfiles[idx];

    ret.internalName = "FILE_";
    ret.internalName += op.title;
    ret.displayName = op.title;
    for (size_t i = 0; i < op.pathList.size(); ++i)
        ret.entries.push_back(op.pathList[i].filename().stem().u8string());
    ret.defaultEntry = op.defIdx;

    return ret;
}

StringPath SoundSetLR2::getFilePath() const
{
    return filePath.is_absolute() ? filePath : filePath.relative_path();
}
