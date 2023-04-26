#include "common/pch.h"
#include "i18n.h"

std::vector<i18n> i18n::languages;
size_t i18n::currentLanguage = 0;

i18n::i18n(const Path& translationFile)
{
    std::string stem = translationFile.stem().u8string();
    if (strEqual(stem, "zh-cn", true))
    {
        type = Languages::ZHCN;
    }

    std::ifstream ifsFile(translationFile);
    std::stringstream ss;
    ss << ifsFile.rdbuf();
    ss.sync();
    ifsFile.close();

    // File is in UTF-8 encoding, no need to convert

    for (size_t i = 0; i < i18n_TEXT_COUNT && !ss.eof(); ++i)
    {
        std::string line;
        std::getline(ss, line);

        static const RE2 regexIn{ "\\\\n" };
        static const re2::StringPiece regexOut{ "\n" };
        RE2::GlobalReplace(&line, regexIn, regexOut);

        text[i] = line;
    }
}

void i18n::init()
{
    // english first..
    Path en = Path(GAMEDATA_PATH) / "resources" / "translations" / "en.txt";
    languages.push_back(i18n(en));

    // other languages
    for (auto& f : fs::directory_iterator(Path(GAMEDATA_PATH) / "resources" / "translations"))
    {
        if (fs::equivalent(f.path(), en)) continue;

        if (strEqual(f.path().extension().u8string(), ".txt"))
        {
            languages.push_back(i18n(f.path()));
        }
    }
}

std::vector<std::string> i18n::getLanguageList()
{
    std::vector<std::string> res;
    for (auto& l : languages)
    {
        res.push_back(l.text[i18nText::LANGUAGE_NAME]);
    }
    return res;
}

void i18n::setLanguage(size_t index)
{
    currentLanguage = index >= languages.size() ? 0 : index;
}

void i18n::setLanguage(const std::string& name)
{
    for (size_t i = 0; i < languages.size(); ++i)
    {
        if (languages[i].text[i18nText::LANGUAGE_NAME] == name)
        {
            currentLanguage = i;
            break;
        }
    }
}

const std::string& i18n::s(size_t index)
{
    static std::string emptyString;
    if (index >= i18n_TEXT_COUNT) 
        return emptyString;

    return languages[currentLanguage].text[index];
}

const char* i18n::c(size_t index)
{
    return s(index).c_str();
}


Languages i18n::getCurrentLanguage()
{
    return languages[currentLanguage].type;
}