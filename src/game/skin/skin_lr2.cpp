#include "skin_lr2.h"
#include "common/log.h"
#include "common/utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <set>
#include <variant>
#include <execution>
#include "skin_lr2_button_callbacks.h"
#include "skin_lr2_slider_callbacks.h"  
#include "game/data/option.h"
#include "game/data/switch.h"
#include "game/graphics/video.h"
#include "game/scene/scene_context.h"
#include "skin_lr2_converters.h"
#include "config/config_mgr.h"
#include "game/scene/scene_customize.h"

#ifdef _WIN32
// For GetWindowsDirectory
#include <Windows.h>
#endif

using namespace std::placeholders;


enum sprite_type
{
    // General
    LR2_IMAGE,
    LR2_NUMBER,
    LR2_SLIDER,
    LR2_BARGRAPH,
    LR2_BUTTON,
    LR2_ONMOUSE,
    LR2_TEXT,

    // Play
    LR2_BGA,
    LR2_JUDGELINE,
    LR2_BARLINE,
    LR2_NOTE,
    LR2_NOWJUDGE_1P,
    LR2_NOWCOMBO_1P,
    LR2_NOWJUDGE_2P,
    LR2_NOWCOMBO_2P,

    // Select
    LR2_BAR_BODY,
    LR2_BAR_FLASH,
    LR2_BAR_LEVEL,
    LR2_BAR_LAMP,
    LR2_BAR_TITLE,
    LR2_BAR_RANK,
    LR2_BAR_RIVAL,
    LR2_README,
    LR2_MOUSECURSOR,

    // Result
    LR2_GAUGECHART_1P,
    LR2_GAUGECHART_2P,
};

namespace lr2skin
{

size_t convertLine(const Tokens& tokens, int* pData, size_t start, size_t count)
{
    size_t end = start + count;

    size_t i;
    for (i = start; i < end && i < tokens.size(); ++i)
    {
        pData[i] = toInt(tokens[i]);
    }
    return i;
}

size_t convertOpsToInt(const Tokens& tokens, int* pData, size_t offset, size_t size)
{
    if (tokens.size() < offset) return 0;

    size_t i;
    for (i = 0; i < size && offset + i < tokens.size(); ++i)
    {
        auto ops = tokens[offset + i];
        if (ops.empty()) continue;

        if (ops[0] == '!' || ops[0] == '-')
            pData[offset + i] = -toInt(ops.substr(1));
        else
            pData[offset + i] = toInt(ops);
    }
    return i;
}

    struct s_basic
    {
        int _null = 0;
        int gr = 0;
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        int div_x = 0;
        int div_y = 0;
        int cycle = 0;
        int timer = 0;
        s_basic(const Tokens& tokens, size_t csvLineNumber = 0)
        {
            int count = sizeof(s_basic) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);

            if (w == -1) w = RECT_FULL.w;
            if (h == -1) h = RECT_FULL.h;

            if (div_x == 0)
            {
                LOG_WARNING << "[Skin] " << csvLineNumber << ": div_x is 0";
                div_x = 1;
            }
            if (div_y == 0)
            {
                LOG_WARNING << "[Skin] " << csvLineNumber << ": div_y is 0";
                div_y = 1;
            }
        }
    };

    struct s_image : s_basic
    {
        int op1 = 0;
        int op2 = 0;
        int op3 = 0;
        s_image(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertOpsToInt(tokens, (int*)this, &op1 - &_null, 3);
        }
    };

    struct s_number : s_basic
    {
        int num = 0;
        int align = 0;
        int keta = 0;
        s_number(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &num - &_null, 3);
        }
    };

    struct s_slider : s_basic
    {
        int muki = 0;
        int range = 0;
        int type = 0;
        int disable = 0;
        s_slider(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &muki - &_null, 4);
        }
    };

    struct s_bargraph : s_basic
    {
        int type = 0;
        int muki = 0;
        s_bargraph(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &type - &_null, 2);
        }
    };

    struct s_button : s_basic
    {
        int type = 0;
        int click = 0;
        int panel = 0;
        int plusonly = 0;
        s_button(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &type - &_null, 4);
        }
    };

    struct s_onmouse : s_basic
    {
        int panel = 0;
        int x2 = 0;
        int y2 = 0;
        int w2 = 0;
        int h2 = 0;
        s_onmouse(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &panel - &_null, 5);
        }
    };

    struct s_text
    {
        int _null = 0;
        int font = 0;
        int st = 0;
        int align = 0;
        int edit = 0;
        int panel = 0;
        s_text(const Tokens& tokens)
        {
            int count = sizeof(s_text) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);
        }
    };

    struct s_bga
    {
        int _null[10] = { 0 };
        int nobase = 0;
        int nolayer = 0;
        int nopoor = 0;
        s_bga(const Tokens& tokens)
        {
            int count = sizeof(s_bga) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);
        }
    };

    typedef s_basic s_judgeline;
    typedef s_basic s_barline;
    typedef s_basic s_note;

    struct s_nowjudge : s_basic
    {
        int noshift = 0;
        s_nowjudge(const Tokens& tokens, size_t csvLineNumber = 0): s_basic(tokens, csvLineNumber)
        {
            size_t offset = sizeof(s_basic) / sizeof(int);
            size_t count = (sizeof(*this) - sizeof(s_basic)) / sizeof(int);
            convertLine(tokens, (int*)this, offset, count);
        }
    };

    struct s_nowcombo : s_basic
    {
        int _null2 = 0;
        int align = 0;
        int keta = 0;
        s_nowcombo(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            size_t offset = sizeof(s_basic) / sizeof(int);
            size_t count = (sizeof(*this) - sizeof(s_basic)) / sizeof(int);
            convertLine(tokens, (int*)this, offset, count);
        }
    };

    struct s_groovegauge : s_basic
    {
        int add_x = 0;
        int add_y = 0;
        s_groovegauge(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            size_t offset = sizeof(s_basic) / sizeof(int);
            size_t count = (sizeof(*this) - sizeof(s_basic)) / sizeof(int);
            convertLine(tokens, (int*)this, offset, count);
        }
    };

    typedef s_basic s_barbody;
    typedef s_basic s_barflash;
    typedef s_nowcombo s_barlevel;
    typedef s_basic s_barlamp;
    struct s_bartitle
    {
        int _null = 0;
        int font = 0;
        int st = 0;
        int align = 0;
        s_bartitle(const Tokens& tokens)
        {
            int count = sizeof(s_bartitle) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);
        }
    };
    typedef s_basic s_barrank;
    typedef s_basic s_barrival;
    struct s_readme
    {
        int _null = 0;
        int font = 0;
        int _null2[2] = { 0 };
        int kankaku = 0;
        s_readme(const Tokens& tokens)
        {
            int count = sizeof(s_readme) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);
        }
    };
    typedef s_basic s_mousecursor;

    struct s_gaugechart : s_basic
    {
        int field_w = 0;
        int field_h = 0;
        int start = 0;
        int end = 0;
        s_gaugechart(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            size_t offset = sizeof(s_basic) / sizeof(int);
            size_t count = (sizeof(*this) - sizeof(s_basic)) / sizeof(int);
            convertLine(tokens, (int*)this, offset, count);
        }
    };
    typedef s_gaugechart s_scorechart;

    struct dst
    {
        int _null = 0;      //0
        int time = 0;       //1
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        int acc = 0;        //6
        int a = 0;
        int r = 0;
        int g = 0;
        int b = 0;
        int blend = 0;      //11
        int filter = 0;     //12
        int angle = 0;      //13
        int center = 0;     //14
        int loop = -1;       //15
        int timer = -1;      //16    
        int op[4]{DST_TRUE, DST_TRUE, DST_TRUE, DST_TRUE};
        dst(const Tokens& tokens)
        {
            int count = sizeof(dst) / sizeof(int);
            convertLine(tokens, (int*)this, 0, count);
            convertOpsToInt(tokens, (int*)this, &op[0] - &_null, sizeof(op) / sizeof(op[0]));
        }
    };

}

std::map<Path, std::shared_ptr<SkinLR2::LR2Font>> SkinLR2::LR2FontCache;

int SkinLR2::setExtendedProperty(std::string&& key, void* value)
{
    if (key == "GAUGETYPE_1P")
    {
        if (gSprites[GLOBAL_SPRITE_IDX_1PGAUGE] != nullptr)
        {
            auto type = *(eGaugeOp*)value;
            auto pS = std::reinterpret_pointer_cast<SpriteGaugeGrid>(gSprites[GLOBAL_SPRITE_IDX_1PGAUGE]);
            switch (type)
            {
            case eGaugeOp::SURVIVAL:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::SURVIVAL);
                break;

            case eGaugeOp::EX_SURVIVAL:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::EX_SURVIVAL);
                break;

            case eGaugeOp::GROOVE:
            default:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::GROOVE);
                break;
            }
        }
        else
            return 1;
        return 0;
    }

    if (key == "GAUGETYPE_2P")
    {
        if (gSprites[GLOBAL_SPRITE_IDX_2PGAUGE] != nullptr)
        {
            auto type = *(eGaugeOp*)value;
            auto pS = std::reinterpret_pointer_cast<SpriteGaugeGrid>(gSprites[GLOBAL_SPRITE_IDX_2PGAUGE]);
            switch (type)
            {
            case eGaugeOp::SURVIVAL:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::SURVIVAL);
                break;

            case eGaugeOp::EX_SURVIVAL:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::EX_SURVIVAL);
                break;

            case eGaugeOp::GROOVE:
            default:
                pS->setGaugeType(SpriteGaugeGrid::GaugeType::GROOVE);
                break;
            }
        }
        else
            return 1;
        return 0;
    }

    return -1;
}


#pragma region Parsing helpers

// For LR2 skin .csv parsing:
// op1~4 may include a '!' before the number, split it out
std::pair<unsigned, bool> toPairUIntBool(const std::string_view& str) noexcept
{
    if (str.empty())
        return { -1, false };

    int val;
    bool notPref;
    if (str[0] == '!')
    {
        val = toInt(str.substr(1), -1);
        notPref = true;
    }
    else
    {
        val = toInt(str, -1);
        notPref = false;
    }

    if (val >= 0)
        return { val, notPref };
    else
        return { -1, false };
}
std::pair<unsigned, bool> toPairUIntBool(const std::string& str) noexcept { return toPairUIntBool(std::string_view(str)); }

std::string_view csvLineNormalize(const std::string& raw)
{
    if (raw.empty()) return {};

    StringContentView linecsv(raw);

    // remove leading spaces
    while (!linecsv.empty() && (linecsv[0] == ' ' || linecsv[0] == '\t'))
    {
        linecsv = linecsv.substr(linecsv.find_first_not_of(' '));
        linecsv = linecsv.substr(linecsv.find_first_not_of('\t'));
    }
    // skip comments
    if (linecsv.substr(0, 2) == "//") return {};
    // skip empty line
    if (linecsv.empty() || linecsv[0] == '\r') return {};
    // remove trailing \r
    if (size_t pos = linecsv.find_last_not_of('\r'); pos != linecsv.npos)
        linecsv = linecsv.substr(0, pos + 1);
    if (linecsv.empty()) return {};

    return linecsv;
}

Tokens csvLineTokenizeSimple(const std::string& raw)
{
    StringContentView linecsv = csvLineNormalize(raw);
    if (linecsv.empty()) return {};

    Tokens res;
    res.reserve(32);
    size_t idx = 0;
    size_t pos = 0;
    do 
    {
        pos = linecsv.find(',', idx);
        auto token = linecsv.substr(idx, pos - idx);
        res.push_back(token);
        idx = pos + 1;
    } while (pos != linecsv.npos);
    return res;
}

Tokens csvLineTokenize(const std::string& raw)
{
    if (raw.find('\\') == raw.npos)
    {
        return csvLineTokenizeSimple(raw);
    }

    StringContentView linecsv = csvLineNormalize(raw);
    if (linecsv.empty()) return {};

    static const std::regex re{ R"(((?:(?:\\,)|[^,])*?)(?:,|$))",
        std::regex_constants::ECMAScript | std::regex_constants::optimize };
    std::match_results<decltype(linecsv.begin())> matchResults;
    typedef std::regex_token_iterator<decltype(linecsv.begin())> scv_regex_token_iterator;
    Tokens res;
    res.reserve(32);
    for (auto it = scv_regex_token_iterator(linecsv.begin(), linecsv.end(), re, 1);
        it != scv_regex_token_iterator() && it->first != linecsv.end(); ++it)
    {
        res.push_back(StringContentView(&*it->first, it->second - it->first));
    }
    return res;
}

Point getCenterPoint(const int& wi, const int& hi, int numpadCenter)
{
    double w = (double)wi;
    double h = (double)hi;
    switch (numpadCenter)
    {
    case 1: return { 0, h };
    case 2: return { w / 2.0, h };
    case 3: return { w, h };
    case 4: return { 0, h / 2.0 };
    case 0:
    case 5: return { w / 2.0, h / 2.0 };
    case 6: return { w, h / 2.0 };
    case 7: return { 0, 0 };
    case 8: return { w / 2.0, 0 };
    case 9: return { w, 0 };
    default: return {};
    }
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// File parsing
#pragma region File parsing
int SkinLR2::IMAGE()
{
    if (strEqual(parseKeyBuf, "#IMAGE", true))
    {
        Path path = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), parseParamBuf[0]);
        StringPath pathStr = path.native();
        std::string pathU8Str = path.u8string();
        if (pathStr.find("*"_p) != pathStr.npos)
        {
            static const std::set<std::string> video_file_extensions =
            {
                ".mpg",
                ".flv",
                ".mp4",
                ".m4p",
                ".m4v",
                ".f4v",
                ".avi",
                ".wmv",
                ".mpeg",
                ".mpeg2",
                ".mkv",
                ".webm",
            };

            // Check if the wildcard path is specified by custom settings
            for (auto& cf : customize)
            {
                if (cf.type == Customize::_Type::FILE && cf.filepath == pathU8Str.substr(0, cf.filepath.length()))
                {
                    const auto& paths = cf.pathList;
                    if (paths.empty())
                    {
                        _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(""));
                        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added IMAGE[" << imageCount << "]: " << "(placeholder)";
                    }
                    else
                    {
                        if (cf.filepath.length() < pathU8Str.length())
                        {
                            // #IMAGE offered a more specific path. Rebuild customfile list
                            auto selection = paths[cf.value];
                            auto defaultSel = paths[cf.defIdx];
                            cf.value = 0;
                            cf.defIdx = 0;

                            auto ls = findFiles(path);
                            for (size_t param = 0; param < ls.size(); ++param)
                            {
                                auto filename = ls[param].filename().stem();
                                if (filename == selection)
                                {
                                    cf.value = param;
                                }
                                if (filename == defaultSel)
                                {
                                    cf.defIdx = param;
                                }
                            }

                            cf.filepath = pathU8Str;
                            cf.label.clear();
                            for (auto& p : ls)
                            {
                                cf.label.push_back(p.filename().stem().u8string());
                            }
                            cf.pathList = std::move(ls);
                        }

                        if (video_file_extensions.find(toLower(paths[cf.value].extension().u8string())) != video_file_extensions.end())
                        {
#ifndef VIDEO_DISABLED
                            _vidNameMap[std::to_string(imageCount)] = std::make_shared<sVideo>(paths[cf.value], true);
                            _textureNameMap[std::to_string(imageCount)] = _textureNameMap["White"];
#else
                            _textureNameMap[std::to_string(imageCount)] = _textureNameMap["Black"];
#endif
                        }
                        else
                        {
                            Image img = Image(paths[cf.value].u8string().c_str());
                            if (info.hasTransparentColor) img.setTransparentColorRGB(info.transparentColor);
                            _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(img);
                        }
                        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added IMAGE[" << imageCount << "]: " << cf.filepath;
                    }
                    ++imageCount;
                    return 2;
                }
            }

            // Or, randomly choose a file
            auto ls = findFiles(path);
            if (ls.empty())
            {
                _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(""));
                //imagePath.push_back(defs::file::errorTextureImage);
                LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added random IMAGE[" << imageCount << "]: " << "(file not found)";
            }
            else
            {
                size_t ranidx = std::rand() % ls.size();
                if (video_file_extensions.find(toLower(ls[ranidx].extension().u8string())) != video_file_extensions.end())
                {
#ifndef VIDEO_DISABLED
                    _vidNameMap[std::to_string(imageCount)] = std::make_shared<sVideo>(ls[ranidx], true);
                    _textureNameMap[std::to_string(imageCount)] = _textureNameMap["Error"];
#else
                    _textureNameMap[std::to_string(imageCount)] = _textureNameMap["Black"];
#endif
                }
                else
                    _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(ls[ranidx].u8string().c_str()));
                LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added random IMAGE[" << imageCount << "]: " << ls[ranidx].u8string();
            }
            ++imageCount;
            return 3;

            // TODO #IMAGE CONTINUE (derive from previous skin)
        }
        else
        {
            // Normal path
            _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(path.u8string().c_str()));
            LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added IMAGE[" << imageCount << "]: " << path.u8string();
        }
        ++imageCount;
        return 1;
    }
    return 0;
}

int SkinLR2::LR2FONT()
{
    if (strEqual(parseKeyBuf, "#LR2FONT", true))
    {
        Path path = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), parseParamBuf[0]);
        path = std::filesystem::absolute(path);
        size_t idx = LR2FontNameMap.size();

        if (LR2FontCache.find(path) != LR2FontCache.end())
        {
            LR2FontNameMap[std::to_string(idx)] = LR2FontCache[path];
            return 1;
        }

        if (!std::filesystem::is_regular_file(path))
        {
            LR2FontNameMap[std::to_string(idx)] = nullptr;
            LOG_WARNING << "[Skin] " << csvLineNumber << ": LR2FONT file not found: " << path.u8string();
            return 0;
        }

        std::ifstream ifsFile(path, std::ios::binary);
        if (ifsFile.fail())
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": LR2FONT file open failed: " << path.u8string();
            return 0;
        }

        // copy the whole file into ram, once for all
        std::stringstream lr2font;
        lr2font << ifsFile.rdbuf();
        lr2font.sync();
        ifsFile.close();

        auto encoding = getFileEncoding(path);

        auto pf = std::make_shared<LR2Font>();

        int lr2fontLineNumber = 0;
        while (!lr2font.eof())
        {
            std::string raw;
            std::getline(lr2font, raw);
            ++lr2fontLineNumber;

            // convert codepage
            raw = to_utf8(raw, encoding);

            auto tokens = csvLineTokenize(raw);
            if (tokens.empty()) continue;
            auto key = tokens[0];

            if (strEqual(key, "#S", true))
            {
                pf->S = toInt(tokens[1]);
            }
            else if (strEqual(key, "#M", true))
            {
                pf->M = toInt(tokens[1]);
            }
            else if (strEqual(key, "#T", true))
            {
                size_t idx = pf->T_id.size();
                pf->T_id[toInt(tokens[1])] = idx;

                // スキンcsvとは違って「lr2fontファイルからの相対参照」で画像ファイルを指定します。
                Path p = path.parent_path() / Path(tokens[2]);
                pf->T_texture.push_back(std::make_shared<Texture>(Image(p.u8string().c_str())));
            }
            else if (strEqual(key, "#R", true))
            {
                int imgId = toInt(tokens[2]);
                if (pf->T_id.find(imgId) == pf->T_id.end()) continue;

                int chrId = toInt(tokens[1]);
                char s_sjis[3]{ 0 };
                char32_t c_utf32 = '\0';
                if (chrId >= 0 && chrId <= 255)
                {
                    // ID = ASCII
                    s_sjis[0] = chrId & 0xFF;
                }
                else if (chrId >= 256 && chrId <= 8126)
                {
                    // ID = Shift-JIS文字コードを10進数に変換して32832を引いた値
                    int i = chrId + 32832;
                    s_sjis[0] = (i >> 8) & 0xFF;
                    s_sjis[1] = i & 0xFF;

                }
                else if (chrId >= 8127 && chrId <= 15306)
                {
                    // ID = Shift-JIS文字コードを10進数に変換して49281を引いてた値
                    int i = chrId + 49281;
                    s_sjis[0] = (i >> 8) & 0xFF;
                    s_sjis[1] = i & 0xFF;
                }
                else continue;

                // convert Shift-JIS to UTF-32
                {
#ifdef _WIN32
                    const static auto locale932 = std::locale(".932");
#else
                    const static auto locale932 = std::locale("ja_JP.SJIS");
#endif
                    Path tmp{ s_sjis, locale932 };
                    c_utf32 = tmp.u32string()[0];
                }

                Rect r;
                r.x = toInt(tokens[3]);
                r.y = toInt(tokens[4]);
                r.w = toInt(tokens[5]);
                r.h = toInt(tokens[6]);

                pf->R[c_utf32] = {pf->T_id.at(imgId), r};
            }
        }

        LR2FontCache[path] = pf;
        LR2FontNameMap[std::to_string(idx)] = pf;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added LR2FONT: " << path.u8string();
        return 1;
    }
    return 0;
}

int SkinLR2::SYSTEMFONT()
{
    // Could not get system font file path in a reliable way while cross-platforming..
    if (strEqual(parseKeyBuf, "#FONT", true))
    {
        int ptsize = toInt(parseParamBuf[0]);
        int thick = toInt(parseParamBuf[1]);
        int fonttype = toInt(parseParamBuf[2]);
        //StringContent name = parseParamBuf[3];
#if defined _WIN32
        TCHAR windir[MAX_PATH];
        GetWindowsDirectory(windir, MAX_PATH);
        std::string fontPath = windir;
        fontPath += "\\Fonts\\msgothic.ttc";
#elif defined LINUX
        StringContent fontPath = "/usr/share/fonts/tbd.ttf"
#endif
        size_t idx = _fontNameMap.size();
        _fontNameMap[std::to_string(idx)] = std::make_shared<TTFFont>(fontPath.c_str(), ptsize);
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added FONT[" << idx << "]: " << fontPath;
        return 1;
    }
    return 0;
}

int SkinLR2::INCLUDE()
{
    if (strEqual(parseKeyBuf, "#INCLUDE", true))
    {
        Path path(parseParamBuf[0]);
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": INCLUDE: " << path.u8string();
        //auto subCsv = SkinLR2(path);
        //if (subCsv._loaded)
        //    _csvIncluded.push_back(std::move(subCsv));
        loadCSV(path);
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": INCLUDE END //" << path.u8string();
        return 1;
    }
    return 0;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Parameters parsing
#pragma region

int SkinLR2::TIMEOPTION()
{
    if (strEqual(parseKeyBuf, "#STARTINPUT", true))
    {
        info.timeIntro = toInt(parseParamBuf[0]);
        if (info.mode == eMode::RESULT || info.mode == eMode::COURSE_RESULT)
        {
            int rank = toInt(parseParamBuf[1]);
            int update = toInt(parseParamBuf[2]);
            if (rank > 0) info.timeResultRank = rank;
            if (update > 0) info.timeResultRecord = update;
            LOG_DEBUG << "[Skin] " << csvLineNumber << ": STARTINPUT " << info.timeIntro << " " << rank << " " << update;
        }

        return 1;
    }

    else if (strEqual(parseKeyBuf, "#LOADSTART", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeStartLoading = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set time colddown before loading: " << time;
        return 3;
    }

    else if (strEqual(parseKeyBuf, "#LOADEND", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeMinimumLoad = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set time colddown after loading: " << time;
        return 4;
    }

    else if (strEqual(parseKeyBuf, "#PLAYSTART", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeGetReady = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set time READY after loading: " << time;
        return 5;
    }

    else if (strEqual(parseKeyBuf, "#CLOSE", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeFailed = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set FAILED time length: " << time;
        return 6;
    }

    else if (strEqual(parseKeyBuf, "#FADEOUT", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeOutro = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set fadeout time length: " << time;
        return 7;
    }

    else if (strEqual(parseKeyBuf, "#SKIP", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeDecideSkip = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set Decide skip time: " << time;
        return 8;
    }

    else if (strEqual(parseKeyBuf, "#SCENETIME", true))
    {
        int time = toInt(parseParamBuf[0]);
        info.timeDecideExpiry = time;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set Decide expiry time: " << time;
        return 9;
    }

    return 0;
}

int SkinLR2::others()
{
    if (strEqual(parseKeyBuf, "#RELOADBANNER", true))
    {
        reloadBanner = true;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set dynamic banner loading";
        return 1;
    }
    if (strEqual(parseKeyBuf, "#TRANSCOLOR", true))
    {
        int r, g, b;
        r = toInt(parseParamBuf[0]);
        g = toInt(parseParamBuf[1]);
        b = toInt(parseParamBuf[2]);
        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        info.hasTransparentColor = true;
        info.transparentColor = Color(r, g, b, 255);
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Set transparent color: " 
            << std::hex << r << ' ' << g << ' ' << b << std::dec;
        return 2;
    }
    if (strEqual(parseKeyBuf, "#FLIPSIDE", true))
    {
        flipSide = true;
        return 3;
    }
    if (strEqual(parseKeyBuf, "#FLIPRESULT", true))
    {
        flipResult = true;
        return 4;
    }
    if (strEqual(parseKeyBuf, "#DISABLEFLIP", true))
    {
        disableFlipResult = true;
        return 5;
    }
    if (strEqual(parseKeyBuf, "#SCRATCH", true))
    {
        int a, b;
        a = !!toInt(parseParamBuf[0]);
        b = !!toInt(parseParamBuf[1]);
        scratchSide1P = a;
        scratchSide2P = b;
        return 6;
    }
    if (strEqual(parseKeyBuf, "#BAR_CENTER", true))
    {
        barCenter = toInt(parseParamBuf[0]);
        return 7;
    }
    if (strEqual(parseKeyBuf, "#BAR_AVAILABLE", true))
    {
        barClickableFrom = toInt(parseParamBuf[0]);
        barClickableTo = toInt(parseParamBuf[1]);
        return 8;
    }
    if (strEqual(parseKeyBuf, "#SETOPTION", true))
    {
        setCustomDstOpt(toInt(parseParamBuf[0]), 0, toInt(parseParamBuf[1]));
        return 9;
    }
    return 0;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Sprite parsing
#pragma region Sprite parsing

bool SkinLR2::SRC()
{
    if (!strEqual(parseKeyBuf.substr(0, 5), "#SRC_", true))
        return false;

    auto name = toUpper(parseKeyBuf.substr(5));
    DefType type = (defTypeName.find(name) != defTypeName.end()) ?
        defTypeName.at(name) : DefType::UNDEF;

    switch (type)
    {
    case DefType::UNDEF:
    case DefType::BAR_BODY_OFF:
    case DefType::BAR_BODY_ON:      return false;

    case DefType::TEXT:
    case DefType::BAR_TITLE:
    case DefType::NOTE:
    case DefType::MINE:
    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
    case DefType::AUTO_NOTE:
    case DefType::AUTO_MINE:
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:    break;

    default:
    {
        // Find texture from map by gr
        int gr = toInt(parseParamBuf[1]);
        std::string gr_key;
        switch (gr)
        {
        case 100: gr_key = "STAGEFILE"; break;
        case 101: gr_key = "BACKBMP"; break;
        case 102: gr_key = "BANNER"; break;
        case 105: gr_key = "THUMBNAIL"; break;
        case 110: gr_key = "Black"; break;
        case 111: gr_key = "White"; break;
        default: gr_key = std::to_string(gr); break;
        }
        if (_vidNameMap.find(gr_key) != _vidNameMap.end())
        {
            textureBuf = _textureNameMap["White"];
            videoBuf = _vidNameMap[gr_key];
            useVideo = true;
        }
        else if (_textureNameMap.find(gr_key) != _textureNameMap.end())
        {
            textureBuf = _textureNameMap[gr_key];
            videoBuf = nullptr;
            useVideo = false;
        }
        else
        {
            textureBuf = _textureNameMap["Error"];
            videoBuf = nullptr;
            useVideo = false;
        }
    }
    break;
    }

    switch (type)
    {
        case DefType::IMAGE:          SRC_IMAGE();               break;
        case DefType::NUMBER:         SRC_NUMBER();              break;
        case DefType::SLIDER:         SRC_SLIDER();              break;
        case DefType::BARGRAPH:       SRC_BARGRAPH();            break;
        case DefType::BUTTON:         SRC_BUTTON();              break;
        case DefType::ONMOUSE:        SRC_ONMOUSE();             break;
        case DefType::JUDGELINE:      SRC_JUDGELINE();           break;
        case DefType::TEXT:           SRC_TEXT();                break;
        case DefType::GROOVEGAUGE:    SRC_GROOVEGAUGE();         break;
        case DefType::NOWJUDGE_1P:    SRC_NOWJUDGE1();           break;
        case DefType::NOWJUDGE_2P:    SRC_NOWJUDGE2();           break;
        case DefType::NOWCOMBO_1P:    SRC_NOWCOMBO1();           break;
        case DefType::NOWCOMBO_2P:    SRC_NOWCOMBO2();           break;
        case DefType::BGA:            SRC_BGA();                 break;
        case DefType::MOUSECURSOR:    SRC_MOUSECURSOR();         break;
        case DefType::GAUGECHART_1P:  SRC_GAUGECHART(0);         break;
        case DefType::GAUGECHART_2P:  SRC_GAUGECHART(1);         break;
        case DefType::SCORECHART:     SRC_SCORECHART();          break;
        case DefType::BAR_BODY:       SRC_BAR_BODY();            break;
        case DefType::BAR_FLASH:      SRC_BAR_FLASH();           break;
        case DefType::BAR_LEVEL:      SRC_BAR_LEVEL();           break;
        case DefType::BAR_LAMP:       SRC_BAR_LAMP();            break;
        case DefType::BAR_TITLE:      SRC_BAR_TITLE();           break;
        case DefType::BAR_RANK:       SRC_BAR_RANK();            break;
        case DefType::BAR_RIVAL:      SRC_BAR_RIVAL();           break;
        case DefType::BAR_MY_LAMP:    SRC_BAR_RIVAL_MYLAMP();    break;
        case DefType::BAR_RIVAL_LAMP: SRC_BAR_RIVAL_RIVALLAMP(); break;

        case DefType::LINE:
        case DefType::NOTE:           
        case DefType::MINE:           
        case DefType::LN_END:         
        case DefType::LN_BODY:        
        case DefType::LN_START:       
        case DefType::AUTO_NOTE:      
        case DefType::AUTO_MINE:      
        case DefType::AUTO_LN_END:    
        case DefType::AUTO_LN_BODY:   
        case DefType::AUTO_LN_START:  SRC_NOTE(type); break;
    };

    return true;
}

ParseRet SkinLR2::SRC_IMAGE()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

#ifndef VIDEO_DISABLED
    if (useVideo && videoBuf && videoBuf->haveVideo)
    {
        auto psv = std::make_shared<SpriteVideo>(d.w, d.h, videoBuf);
        _sprites.push_back(psv);
    }
    else
#endif
    {
        _sprites.push_back(std::make_shared<SpriteAnimated>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x));
    }

    _sprites.back()->setSrcLine(csvLineNumber);
    
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NUMBER()
{
    lr2skin::s_number d(parseParamBuf, csvLineNumber);

    eNumber iNum = lr2skin::num(d.num);

    // get NumType from div_x, div_y
    unsigned f = d.div_y * d.div_x;
    if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
    else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
    else if (f % NumberType::NUM_TYPE_FULL == 0)
    {
        f = f / NumberType::NUM_TYPE_FULL;
        d.keta++;   //LR2SkinHelp: スキン関連ドキュメントには「ketaは+-文字を含めた桁数です」と記述されていますが、正しくは「 + -符号を含まない桁数」指定です。
    }
    else f = 0;

    _sprites.emplace_back(std::make_shared<SpriteNumber>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (eTimer)d.timer, f));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_SLIDER()
{
    lr2skin::s_slider d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteSlider>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (SliderDirection)d.muki, d.range, 
        d.disable ? lr2skin::slider::getSliderCallback(-1) : lr2skin::slider::getSliderCallback(d.type),
        d.div_y*d.div_x, d.cycle, (eSlider)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);
    
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BARGRAPH()
{
    lr2skin::s_bargraph d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteBargraph>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (BargraphDirection)d.muki, d.div_y*d.div_x, d.cycle, (eBargraph)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BUTTON()
{
    lr2skin::s_button d(parseParamBuf, csvLineNumber);
    
    if (d.type < 270)
    {
        std::shared_ptr<SpriteOption> s;

        if (d.click)
        {
            s = std::make_shared<SpriteButton>(
                textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, lr2skin::button::getButtonCallback(d.type), d.panel, d.plusonly, eTimer::SCENE_START, d.div_y, d.div_x, false);
        }
        else
        {
            s = std::make_shared<SpriteOption>(
                textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
        }
        eSwitch sw;
        eOption op;
        if (lr2skin::buttonSw(d.type, sw))
        {
            if (sw == eSwitch::_TRUE)
            {
                switch (d.type)
                {
                case 72:	// bga off/on/autoplay only, special
                case 73:	// bga normal/extend, special
                case 80:	// window mode, windowed/fullscreen
                case 82:	// vsync
                default:
                    break;
                }
            }
            s->setInd(SpriteOption::opType::SWITCH, (unsigned)sw);
        }
        if (lr2skin::buttonOp(d.type, op))
        {
            s->setInd(SpriteOption::opType::OPTION, (unsigned)op);
        }
        _sprites.push_back(s);
        _sprites.back()->setSrcLine(csvLineNumber);
    }
    else
    {
        // deal as eSwitch::_FALSE
        auto s = std::make_shared<SpriteOption>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
        s->setInd(SpriteOption::opType::SWITCH, (unsigned)eSwitch::_FALSE);
        _sprites.push_back(s);
        _sprites.back()->setSrcLine(csvLineNumber);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_ONMOUSE()
{
    lr2skin::s_onmouse d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteOnMouse>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, d.panel, Rect(d.x2, d.y2, d.w2, d.h2), (eTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_MOUSECURSOR()
{
    lr2skin::s_mousecursor d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteCursor>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_TEXT()
{
    lr2skin::s_text d(parseParamBuf);

    auto font = std::to_string(d.font);
    if (LR2FontNameMap.find(font) != LR2FontNameMap.end() && LR2FontNameMap[font] != nullptr)
    {
        auto& pf = LR2FontNameMap[font];
        _sprites.push_back(std::make_shared<SpriteImageText>(
            pf->T_texture, pf->R, (eText)d.st, (TextAlign)d.align, pf->S, pf->M));
    }
    else
    {
        _sprites.push_back(std::make_shared<SpriteText>(
            _fontNameMap[std::to_string(d.font)], (eText)d.st, (TextAlign)d.align));
    }

    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_GAUGECHART(int player)
{
    lr2skin::s_gaugechart d(parseParamBuf, csvLineNumber);

    LineType type = LineType::GAUGE_F;
    switch (d._null)
    {
    case 0: type = LineType::GAUGE_F; break;
    case 1: type = LineType::GAUGE_C; break;
    default: break;
    }

    _sprites.push_back(std::make_shared<SpriteLine>(
        player == 0 ? PLAYER_SLOT_1P : PLAYER_SLOT_2P,
        type,
        d.field_w, d.field_h, d.start, d.end, 1));

    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_SCORECHART()
{
    lr2skin::s_gaugechart d(parseParamBuf, csvLineNumber);

    LineType type = LineType::SCORE;
    Color color;
    switch (d._null)
    {
    case 0:type = LineType::SCORE; color = {20, 20, 255, 255};  break;
    case 1: type = LineType::SCORE; color = { 20, 255, 20, 255 };   break;
    case 2: type = LineType::SCORE_TARGET; color = { 255, 20, 20, 255 };   break;
    default: break;
    }

    _sprites.push_back(std::make_shared<SpriteLine>(
        0, 
        type,
        d.field_w, d.field_h, d.start, d.end, d.w, color));

    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}


#pragma region SRC: Play skin specified

ParseRet SkinLR2::SRC_JUDGELINE()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    int spriteIdx = -1;
    switch (d._null)
    {
    case 0: spriteIdx = GLOBAL_SPRITE_IDX_1PJUDGELINE; break;
    case 1: spriteIdx = GLOBAL_SPRITE_IDX_2PJUDGELINE; break;
    default: break;
    }
    if (spriteIdx == -1)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Judgeline index invalid (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

    gSprites[spriteIdx] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[spriteIdx]->setSrcLine(csvLineNumber);

    auto p = std::make_shared<SpriteGlobal>(spriteIdx);
    _sprites.push_back(p);
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWJUDGE(size_t idx)
{
    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Nowjudge idx out of range (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    gSprites[idx] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[idx]->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWCOMBO(size_t idx)
{
    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Nowjudge idx out of range (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_number d(parseParamBuf, csvLineNumber);

    eNumber iNum = lr2skin::num(d.num);

    // get NumType from div_x, div_y
    unsigned f = d.div_y * d.div_x;
    if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
    else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
    else if (f % NumberType::NUM_TYPE_FULL == 0) f = f / NumberType::NUM_TYPE_FULL;
    else f = 0;

    gSprites[idx] = std::make_shared<SpriteNumber>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (eTimer)d.timer, f);
    gSprites[idx]->setSrcLine(csvLineNumber);
    std::reinterpret_pointer_cast<SpriteNumber>(gSprites[idx])->setInhibitZero(true);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_GROOVEGAUGE()
{
    lr2skin::s_groovegauge d(parseParamBuf, csvLineNumber);

    if (d.div_y * d.div_x < 4)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": div not enough (Line " << csvLineNumber << ")";
        return ParseRet::DIV_NOT_ENOUGH;
    }

    size_t idx = d._null == 0 ? GLOBAL_SPRITE_IDX_1PGAUGE : GLOBAL_SPRITE_IDX_2PGAUGE;
    eNumber en = d._null == 0 ? eNumber::PLAY_1P_GROOVEGAUGE : eNumber::PLAY_2P_GROOVEGAUGE;

    gSprites[idx] = std::make_shared<SpriteGaugeGrid>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x / 4, d.cycle, d.add_x, d.add_y, 0, 100, 50,
        (eTimer)d.timer, en, d.div_y, d.div_x);
    gSprites[idx]->setSrcLine(csvLineNumber);

    auto p = std::make_shared<SpriteGlobal>(idx);
    _sprites.push_back(p);
    _sprites.back()->setSrcLine(csvLineNumber);
    
    return ParseRet::OK;
}


ParseRet SkinLR2::SRC_NOWJUDGE1()
{
    lr2skin::s_nowjudge d(parseParamBuf, csvLineNumber);

    bufJudge1PSlot = d._null;
    if (bufJudge1PSlot >= 0 && bufJudge1PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_1PJUDGE + bufJudge1PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            noshiftJudge1P[bufJudge1PSlot] = toInt(parseParamBuf[10]);
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites.back()->setSrcLine(csvLineNumber);
        }
        else
        {
            return ret;
        }
    }
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWJUDGE2()
{
    lr2skin::s_nowjudge d(parseParamBuf, csvLineNumber);

    bufJudge2PSlot = d._null;
    if (bufJudge2PSlot >= 0 && bufJudge2PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_2PJUDGE + bufJudge2PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            noshiftJudge2P[bufJudge2PSlot] = toInt(parseParamBuf[10]);
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites.back()->setSrcLine(csvLineNumber);
        }
        else
        {
            return ret;
        }
    }
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWCOMBO1()
{
    lr2skin::s_nowcombo d(parseParamBuf, csvLineNumber);

    bufJudge1PSlot = d._null;
    // modify necessary info for SRC_NOWCOMBO(idx)
    static StringContent eNumBuf = std::to_string((int)eNumber::_DISP_NOWCOMBO_1P);
    parseParamBuf[10] = StringContentView(eNumBuf);
    switch (toInt(parseParamBuf[11]))
    {
    case 0: parseParamBuf[11] = "1"; break;
    case 1: parseParamBuf[11] = "2"; break;
    case 2:
    default:parseParamBuf[11] = "0"; break;
    }

    if (bufJudge1PSlot >= 0 && bufJudge1PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_1PJUDGENUM + bufJudge1PSlot;
        auto ret = SRC_NOWCOMBO(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites.back()->setSrcLine(csvLineNumber);
        }
        else
        {
            return ret;
        }
    }
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWCOMBO2()
{
    lr2skin::s_nowcombo d(parseParamBuf, csvLineNumber);

    bufJudge2PSlot = d._null;
    // modify necessary info for SRC_NOWCOMBO(idx)
    static StringContent eNumBuf = std::to_string((int)eNumber::_DISP_NOWCOMBO_2P);
    parseParamBuf[10] = StringContentView(eNumBuf);
    switch (toInt(parseParamBuf[11]))
    {
    case 0: parseParamBuf[11] = "1"; break;
    case 1: parseParamBuf[11] = "2"; break;
    case 2: 
    default:parseParamBuf[11] = "0"; break;
    }

    if (bufJudge2PSlot >= 0 && bufJudge2PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_2PJUDGENUM + bufJudge2PSlot;
        auto ret = SRC_NOWCOMBO(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites.back()->setSrcLine(csvLineNumber);
        }
        else
        {
            return ret;
        }
    }
    return ParseRet::OK;
}

const size_t NoteIdxToLaneMap[] =
{
    chart::Sc1,
    chart::K1,
    chart::K2,
    chart::K3,
    chart::K4,
    chart::K5,
    chart::K6,
    chart::K7,
    chart::_,
    chart::_,

    chart::Sc2,
    chart::K8,
    chart::K9,
    chart::K10,
    chart::K11,
    chart::K12,
    chart::K13,
    chart::K14,
    chart::_,
    chart::_,
};

ParseRet SkinLR2::SRC_NOTE(DefType type)
{
    using namespace chart;

    // load raw into data struct
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    eTimer iTimer = lr2skin::timer(d.timer);

    // Find texture from map by gr
    pTexture tex = nullptr;
    std::string gr_key = std::to_string(d.gr);
    if (_textureNameMap.find(gr_key) != _textureNameMap.end())
    {
        tex = _textureNameMap[gr_key];
    }
    else
    {
        tex = _textureNameMap["Error"];
    }

    // SRC
    if (d._null >= 20)
    {
        return ParseRet::PARAM_INVALID;
    }

    NoteLaneCategory cat = NoteLaneCategory::_;
    NoteLaneIndex idx = NoteLaneIndex::_;

    // SRC_NOTE
    switch (type)
    {
    case DefType::LINE:
        cat = NoteLaneCategory::EXTRA;
        idx = (NoteLaneIndex)EXTRA_BARLINE;
        break;
    case DefType::NOTE:
        cat = NoteLaneCategory::Note;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        break;
    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
        cat = NoteLaneCategory::LN;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        break;
    case DefType::MINE:
        cat = NoteLaneCategory::Mine;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        break;
    case DefType::AUTO_NOTE:
    case DefType::AUTO_MINE:
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:
    default:
        LOG_WARNING << "[Skin] " << csvLineNumber << ": \"" << parseKeyBuf << "\" is not supported yet";
        return ParseRet::OK;
    }

    size_t i = channelToIdx(cat, idx);
    if (i == LANE_INVALID)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Note channel illegal: " << unsigned(cat) << ", " << unsigned(idx);
        return ParseRet::PARAM_INVALID;
    }

    switch (type)
    {
    case DefType::LINE:
    case DefType::NOTE:
    case DefType::MINE:
    case DefType::AUTO_NOTE:
    case DefType::AUTO_MINE:
    {
        _sprites.push_back(std::make_shared<SpriteLaneVertical>(
            _textureNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x, false, !!(d._null >= 20)));

        _laneSprites[i] = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
        _laneSprites[i]->setLane(cat, idx);
        _laneSprites[i]->pNote->appendKeyFrame({ 0, {Rect(),
            RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0.0 } });
        _laneSprites[i]->pNote->setLoopTime(0);
        break;
    }

    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:
    {
        if (_laneSprites[i] == nullptr)
        {
            _sprites.push_back(std::make_shared<SpriteLaneVerticalLN>(!!(d._null >= 20)));
            _laneSprites[i] = std::static_pointer_cast<SpriteLaneVerticalLN>(_sprites.back());
            _laneSprites[i]->setLane(cat, idx);
        }

        auto p = std::static_pointer_cast<SpriteLaneVerticalLN>(_laneSprites[i]);
        std::shared_ptr<SpriteAnimated> *pn = nullptr;
        switch (type)
        {
        case DefType::LN_START:
        case DefType::AUTO_LN_START:  pn = &p->pNote; break;
        case DefType::LN_BODY:
        case DefType::AUTO_LN_BODY:   pn = &p->pNoteBody; break;
        case DefType::LN_END:
        case DefType::AUTO_LN_END:    pn = &p->pNoteTail; break;
        }
        if (pn)
        {
            *pn = std::make_shared<SpriteAnimated>(
                _textureNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x);
            (*pn)->appendKeyFrame({ 0, {Rect(), RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0.0 } });
            (*pn)->setLoopTime(0);
        }
        break;
    }
    default:
        break;
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BGA()
{
    lr2skin::s_bga d(parseParamBuf);

    _sprites.push_back(std::make_shared<SpriteStatic>(gPlayContext.bgaTexture, Rect()));

    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}


#pragma endregion

#pragma region SRC: Select skin specified

ParseRet SkinLR2::SRC_BAR_BODY()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarType type;
    switch (d._null & 0xFFFFFFFF)
    {
    case 0:  type = BarType::SONG; break;
        //   type = BarType::NEW_SONG; break;
    case 1:  type = BarType::FOLDER; break;
    case 2:  type = BarType::CUSTOM_FOLDER; break;
    case 3:  type = BarType::NEW_SONG_FOLDER; break;
    case 4:  type = BarType::RIVAL; break;
    case 5:  type = BarType::SONG_RIVAL; break;
    case 6:  type = BarType::COURSE_FOLDER; break;
    case 7:  type = BarType::NEW_COURSE; break;
    case 8:  type = BarType::COURSE; break;
    case 9:  type = BarType::RANDOM_COURSE; break;
    case 10: type = BarType::TYPE_COUNT; break;
    default: break;
    }

    for (auto& bar : _barSprites)
    {
        bar->setBody(type,
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);

        if (type == BarType::SONG)
            bar->setBody(BarType::NEW_SONG,
                textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_FLASH()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    for (auto& bar : _barSprites)
    {
        bar->setFlash(textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_LEVEL()
{
    lr2skin::s_number d(parseParamBuf, csvLineNumber);

    BarLevelType type = BarLevelType(d._null & 0xFFFFFFFF);

    // get NumType from div_x, div_y
    unsigned f = d.div_y * d.div_x;
    if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
    else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
    else if (f % NumberType::NUM_TYPE_FULL == 0) f = f / NumberType::NUM_TYPE_FULL;
    else f = 0;

    for (auto& bar : _barSprites)
    {
        bar->setLevel(type,
            textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, (eTimer)d.timer, f);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_LAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setLamp(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_TITLE()
{
    lr2skin::s_text d(parseParamBuf);
    BarTitleType type = BarTitleType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        auto font = std::to_string(d.font);
        if (LR2FontNameMap.find(font) != LR2FontNameMap.end() && LR2FontNameMap[font] != nullptr)
        {
            auto& pf = LR2FontNameMap[font];
            bar->setTitle(type, pf->T_texture, pf->R, (TextAlign)d.align, pf->S, pf->M);
        }
        else
        {
            bar->setTitle(type, _fontNameMap[font], (TextAlign)d.align);
        }
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RANK()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarRankType type = BarRankType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRank(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarRivalType type = BarRivalType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalWinLose(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_MYLAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampSelf(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_RIVALLAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampRival(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}


#pragma endregion

bool SkinLR2::DST()
{
    if (!strEqual(parseKeyBuf.substr(0, 5), "#DST_", true))
        return false;

    auto name = toUpper(parseKeyBuf.substr(5));
    DefType type = (defTypeName.find(name) != defTypeName.end()) ?
        defTypeName.at(name) : DefType::UNDEF;

    switch (type)
    {
    case DefType::UNDEF:
    case DefType::BAR_BODY:
    case DefType::MINE:
    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
    case DefType::AUTO_NOTE:
    case DefType::AUTO_MINE:
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:  return false;

    case DefType::BAR_BODY_OFF:   
    case DefType::BAR_BODY_ON:    DST_BAR_BODY();            break;
    case DefType::BAR_FLASH:      DST_BAR_FLASH();           break;
    case DefType::BAR_LEVEL:      DST_BAR_LEVEL();           break;
    case DefType::BAR_LAMP:       DST_BAR_LAMP();            break;
    case DefType::BAR_TITLE:      DST_BAR_TITLE();           break;
    case DefType::BAR_RANK:       DST_BAR_RANK();            break;
    case DefType::BAR_RIVAL:      DST_BAR_RIVAL();           break;
    case DefType::BAR_MY_LAMP:    DST_BAR_RIVAL_MYLAMP();    break;
    case DefType::BAR_RIVAL_LAMP: DST_BAR_RIVAL_RIVALLAMP(); break;

    case DefType::LINE:           DST_LINE();                break;
    case DefType::NOTE:           DST_NOTE();                break;

    default:
    {
        // load raw into data struct
        lr2skin::dst d(parseParamBuf);

        int ret = 0;
        auto e = _sprites.back();
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous src definition invalid (Line: " << csvLineNumber << ")";
            return false;
        }

        // TODO check if type of previous src definition matches

        if (e->type() == SpriteTypes::GLOBAL)
        {
            // unpack stacking references
            auto p = std::reinterpret_pointer_cast<SpriteGlobal>(e);
            auto enext = e;
            do
            {
                enext = gSprites[p->getIdx()];
                p->set(enext);

                if (enext == nullptr)
                {
                    LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous src definition invalid (Line: " << csvLineNumber << ")";
                    return false;
                }

            } while (enext->type() == SpriteTypes::GLOBAL);
        }

        if (e->isKeyFrameEmpty())
        {
            switch (type)
            {
            case DefType::NOWCOMBO_1P:
            case DefType::NOWJUDGE_1P:
                switch (bufJudge1PSlot)
                {
                case 0: d.timer = (int)eTimer::_JUDGE_1P_0; break;
                case 1: d.timer = (int)eTimer::_JUDGE_1P_1; break;
                case 2: d.timer = (int)eTimer::_JUDGE_1P_2; break;
                case 3: d.timer = (int)eTimer::_JUDGE_1P_3; break;
                case 4: d.timer = (int)eTimer::_JUDGE_1P_4; break;
                case 5: d.timer = (int)eTimer::_JUDGE_1P_5; break;
                default: break;
                }
                break;

            case DefType::NOWCOMBO_2P:
            case DefType::NOWJUDGE_2P:
                switch (bufJudge2PSlot)
                {
                case 0: d.timer = (int)eTimer::_JUDGE_2P_0; break;
                case 1: d.timer = (int)eTimer::_JUDGE_2P_1; break;
                case 2: d.timer = (int)eTimer::_JUDGE_2P_2; break;
                case 3: d.timer = (int)eTimer::_JUDGE_2P_3; break;
                case 4: d.timer = (int)eTimer::_JUDGE_2P_4; break;
                case 5: d.timer = (int)eTimer::_JUDGE_2P_5; break;
                default: break;
                }
                break;
            }

            std::vector<dst_option> opEx;
            if (type == DefType::NUMBER)
            {
                auto p = std::reinterpret_pointer_cast<SpriteNumber>(e);
                switch (p->_numInd)
                {
                case eNumber::MUSIC_BEGINNER_LEVEL:  opEx.push_back(dst_option::SELECT_HAVE_BEGINNER_IN_SAME_FOLDER); break;
                case eNumber::MUSIC_NORMAL_LEVEL:    opEx.push_back(dst_option::SELECT_HAVE_NORMAL_IN_SAME_FOLDER);   break;
                case eNumber::MUSIC_HYPER_LEVEL:     opEx.push_back(dst_option::SELECT_HAVE_HYPER_IN_SAME_FOLDER);    break;
                case eNumber::MUSIC_ANOTHER_LEVEL:   opEx.push_back(dst_option::SELECT_HAVE_ANOTHER_IN_SAME_FOLDER);  break;
                case eNumber::MUSIC_INSANE_LEVEL:    opEx.push_back(dst_option::SELECT_HAVE_INSANE_IN_SAME_FOLDER);   break;
                }
            }
            drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), opEx });

            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        if (type == DefType::GAUGECHART_1P || type == DefType::GAUGECHART_2P)
        {
            switch (d._null)
            {
            case 0: d.r = d.b = 0; break;   // green
            case 1: d.g = d.b = 0; break;   // red
            }
        }
        else if (type == DefType::SCORECHART)
        {
            switch (d._null)
            {
            case 0: d.r = d.g = 0; break;   // blue
            case 1: d.r = d.b = 0; break;   // green
            case 2: d.g = d.b = 0; break;   // red
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
        //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
        //LOG_DEBUG << "[Skin] " << raw << ": Set sprite Keyframe (time: " << d.time << ")";

    }
    break;

    }

    return true;
}


ParseRet SkinLR2::DST_NOTE()
{
    using namespace chart;

    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    if (d._null >= 20)
        return ParseRet::PARAM_INVALID;

    NoteLaneIndex idx = NoteLaneIndex(NoteIdxToLaneMap[d._null]);

    auto setDstNoteSprite = [&](NoteLaneCategory i, std::shared_ptr<SpriteLaneVertical> e)
    {
        /*
        e->pNote->clearKeyFrames();
        e->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center)  } });
            */
        
        drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
        e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
        e->setLoopTime(0);
        //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
        //LOG_DEBUG << "[Skin] " << raw << ": Set Lane sprite Keyframe (time: " << d.time << ")";
    };

    auto e1 = _laneSprites[channelToIdx(NoteLaneCategory::Note, idx)];
    if (e1) setDstNoteSprite(NoteLaneCategory::Note, e1);

    auto e2 = _laneSprites[channelToIdx(NoteLaneCategory::Mine, idx)];
    if (e2) setDstNoteSprite(NoteLaneCategory::Mine, e2);

    auto e3 = _laneSprites[channelToIdx(NoteLaneCategory::LN, idx)];
    if (e3) setDstNoteSprite(NoteLaneCategory::LN, e3);

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_LINE()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    

    int ret = 0;
    auto e = _sprites.back();
    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Barline SRC definition invalid " <<
            "(Line: " << csvLineNumber << ")";
        return ParseRet::SRC_DEF_INVALID;
    }

    if (e->type() != SpriteTypes::NOTE_VERT)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Barline SRC definition is not NOTE " <<
            "(Line: " << csvLineNumber << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }
    if (!e->isKeyFrameEmpty())
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Barline DST is already defined " <<
            "(Line: " << csvLineNumber << ")";
        e->clearKeyFrames();
    }

    // set sprite channel
    auto p = std::static_pointer_cast<SpriteLaneVertical>(e);

    p->playerSlot = d._null / 10;  // player slot, 1P:0, 2P:1

    chart::NoteLaneCategory cat = p->getLaneCat();
    chart::NoteLaneIndex idx = p->getLaneIdx();
    if (cat != chart::NoteLaneCategory::EXTRA || idx != chart::EXTRA_BARLINE)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous SRC definition is not LINE " <<
            "(Line: " << csvLineNumber << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }

    p->pNote->clearKeyFrames();
    p->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

    drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
    e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    e->setLoopTime(0);
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << raw << ": Set Lane sprite (Barline) Keyframe (time: " << d.time << ")";

    _noteAreaHeight = d.y;

    return ParseRet::OK;
}


ParseRet SkinLR2::DST_BAR_BODY()
{
    bool bodyOn = parseKeyBuf == "#DST_BAR_BODY_ON";

    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    unsigned idx = unsigned(d._null);

    for (BarType type = BarType(0); type != BarType::TYPE_COUNT; ++*(unsigned*)&type)
    {
        auto e = bodyOn ? _barSprites[idx]->getSpriteBodyOn(type) : _barSprites[idx]->getSpriteBodyOff(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_BODY undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);

            if (!_barSpriteAdded[idx])
            {
                _barSprites[idx]->setSrcLine(csvLineNumber);
                _barSpriteAdded[idx] = true;
                drawQueue.push_back({ _barSprites[idx], dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    }

    _barSprites[idx]->pushPartsOrder(bodyOn ? BarPartsType::BODY_ON : BarPartsType::BODY_OFF);

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_FLASH()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteFlash();
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_FLASH undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::FLASH);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LEVEL()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    BarLevelType type = BarLevelType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteLevel(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_LEVEL " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LEVEL);
    }


    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL_MYLAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalLampSelf(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_MY_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::MYLAMP);
    }

    return ParseRet::OK;
}
ParseRet SkinLR2::DST_BAR_RIVAL_RIVALLAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalLampRival(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_RIVAL_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RIVALLAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteLamp(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_TITLE()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    auto type = BarTitleType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteTitle(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_TITLE undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::TITLE);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RANK()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    auto type = BarRankType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRank(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_RANK " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RANK);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);
    
    auto type = BarRivalType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalWinLose(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": SRC_BAR_RIVAL " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            e->setSrcLine(csvLineNumber);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RIVAL);
    }

    return ParseRet::OK;
}



#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Dispatcher
#pragma region Dispatcher

int SkinLR2::parseHeader(const Tokens& raw)
{
    if (raw.empty()) return 0;
    parseParamBuf.resize(raw.size() - 1);
    parseKeyBuf = raw[0];
    for (size_t idx = 0; idx < parseParamBuf.size(); ++idx)
        parseParamBuf[idx] = raw[idx + 1];

    if (strEqual(parseKeyBuf, "#INFORMATION", true))
    {
        while (parseParamBuf.size() < 4) parseParamBuf.push_back("");

        int type = toInt(parseParamBuf[0]);
        StringContent title = StringContent(parseParamBuf[1]);
        StringContent maker = StringContent(parseParamBuf[2]);
        Path thumbnail(parseParamBuf[3]);

        switch (type)
        {
        case 0:		info.mode = eMode::PLAY7;	break;
        case 1:		info.mode = eMode::PLAY5;	break;
        case 2:		info.mode = eMode::PLAY14;	break;
        case 3:		info.mode = eMode::PLAY10;	break;
        case 4:		info.mode = eMode::PLAY9;	break;
        case 5:		info.mode = eMode::MUSIC_SELECT;	break;
        case 6:		info.mode = eMode::DECIDE;	break;
        case 7:		info.mode = eMode::RESULT;	break;
        case 8:		info.mode = eMode::KEY_CONFIG;	break;
        case 9:		info.mode = eMode::THEME_SELECT;	break;
        case 10:	info.mode = eMode::SOUNDSET;	break;
        case 12:	info.mode = eMode::PLAY5_2;	break;
        case 13:	info.mode = eMode::PLAY7_2;	break;
        case 15:	info.mode = eMode::COURSE_RESULT;	break;

        case 17:	info.mode = eMode::TITLE;	break;
        case 16:	info.mode = eMode::PLAY9_2;	break;
            // 11: THEME
            // 14: COURSE EDIT
            // 18: MODE SELECT
            // 19: MODE DECIDE
            // 20: COURSE SELECT
        }
        info.name = title;
        info.maker = maker;

        _textureNameMap["THUMBNAIL"] = std::make_shared<Texture>(Image(
            convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), thumbnail.u8string().c_str())));
        if (_textureNameMap["THUMBNAIL"] == nullptr)
            LOG_WARNING << "[Skin] " << csvLineNumber << ": thumbnail loading failed: " << thumbnail.u8string();

        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Loaded metadata: " << title << " | " << maker;

        return 1;
    }

    else if (strEqual(parseKeyBuf, "#CUSTOMOPTION", true))
    {
        StringContent title(parseParamBuf[0]);
        int dst_op = toInt(parseParamBuf[1]);
        if (dst_op < 900 || dst_op > 999)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": Invalid option value: " << dst_op;
            return -2;
        }
        std::vector<StringContent> op_label;
        for (size_t idx = 2; idx < parseParamBuf.size() && !parseParamBuf[idx].empty(); ++idx)
            op_label.push_back(StringContent(parseParamBuf[idx]));

        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Loaded Custom option " << title << ": " << dst_op;

        Customize c;
        c.type = Customize::_Type::OPT;
        c.title = title;
        c.dst_op = dst_op;
        c.label = std::move(op_label);
        c.value = 0;
        customize.push_back(c);

        return 2;
    }

    else if (strEqual(parseKeyBuf, "#CUSTOMFILE", true))
    {
        StringContent title(parseParamBuf[0]);
        StringContent path(parseParamBuf[1]);
        Path pathf = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), path);
        Path def(parseParamBuf[2]);

        auto ls = findFiles(pathf);
        size_t defVal = 0;
        for (size_t param = 0; param < ls.size(); ++param)
        {
            if (ls[param].filename().stem() == def)
            {
                defVal = param;
                break;
            }
        }

        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Loaded Custom file " << title << ": " << pathf.u8string();

        Customize c;
        c.type = Customize::_Type::FILE;
        c.title = title;
        c.dst_op = 0;
        c.filepath = pathf.u8string();
        for (auto& p : ls)
        {
            c.label.push_back(p.filename().stem().u8string());
        }
        c.pathList = std::move(ls);
        c.defIdx = defVal;
        c.value = defVal;
        customize.push_back(c);

        return 3;
    }

    else if (strEqual(parseKeyBuf, "#ENDOFHEADER", true))
    {
        return -1;
    }

    return 0;
}

int SkinLR2::parseBody(const Tokens &raw)
{
    if (raw.empty()) return 0;
    parseParamBuf.resize(raw.size() - 1);
    parseKeyBuf = raw[0];
    if (parseKeyBuf.empty()) return 0;
    for (size_t idx = 0; idx < parseParamBuf.size(); ++idx)
        parseParamBuf[idx] = raw[idx + 1];

    try {
        if (IMAGE())
            return 1;
        if (LR2FONT())
            return 2;
        if (SYSTEMFONT())
            return 3;
        if (INCLUDE())
            return 4;
        if (TIMEOPTION())
            return 5;
        if (others())
            return 6;
        if (SRC())
            return 7;
        if (DST())
            return 8;

        LOG_WARNING << "[Skin] " << csvLineNumber << ": Invalid def \"" << parseKeyBuf << "\" (Line " << csvLineNumber << ")";
    }
    catch (std::invalid_argument e)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Invalid Argument: " << "(Line " << csvLineNumber << ")";
    }
    catch (std::out_of_range e)
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Out of range: " << "(Line " << csvLineNumber << ")";
    }
    return 0;
}

void SkinLR2::IF(const Tokens &t, std::istream& lr2skin)
{
    bool isElseStmt = false;
    bool ifStmtTrue = true;
    if (strEqual(t[0], "#ELSE", true))
    {
        isElseStmt = true;
    }
    else 
    {
        if (t.size() <= 1)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": No IF parameters " << " (Line " << csvLineNumber << ")";
        }

        // get dst indexes
        for (auto it = ++t.begin(); it != t.end() && ifStmtTrue; ++it)
        {
            if (it->empty()) continue;

            auto [idx, val] = toPairUIntBool(*it);
            if (idx == -1)
            {
                LOG_WARNING << "[Skin] " << csvLineNumber << ": Invalid DST_OPTION Index, deal as false (Line " << csvLineNumber << ")";
                ifStmtTrue = false;
                break;
            }
            bool dst = getDstOpt((dst_option)idx);
            if (val) dst = !dst;
            ifStmtTrue = ifStmtTrue && dst;
        }
    }

    if (ifStmtTrue)
    {
        bool ifBlockEnded = false;
        while (!lr2skin.eof())
        {
            std::string raw;
            std::getline(lr2skin, raw);
            ++csvLineNumber;
            auto tokens = csvLineTokenize(raw);
            if (tokens.empty()) continue;

            if (strEqual(*tokens.begin(), "#ENDIF", true))
            {
                // end #IF process
                return;
            }
            else if (isElseStmt || strEqual(*tokens.begin(), "#ELSEIF", true) || strEqual(*tokens.begin(), "#ELSE", true))
            {
                ifBlockEnded = true;
            }
            else if (!ifBlockEnded)
            {
                // parse current branch
                parseBody(tokens);
            }
        }
    }
    else
    {
        while (!lr2skin.eof())
        {
            std::string raw;
            std::getline(lr2skin, raw);
            ++csvLineNumber;
            auto tokens = csvLineTokenize(raw);
            if (tokens.empty()) continue;

            if (strEqual(*tokens.begin(), "#ELSE", true))
            {
                IF(tokens, lr2skin);
                return;
            }
            else if (strEqual(*tokens.begin(), "#ELSEIF", true))
            {
                IF(tokens, lr2skin);
                return;
            }
            else if (strEqual(*tokens.begin(), "#ENDIF", true))
                return;
        }
    }
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

SkinLR2::SkinLR2(Path p, bool headerOnly)
{
    _type = eSkinType::LR2;

    for (size_t i = 0; i < BAR_ENTRY_SPRITE_COUNT; ++i)
    {
        _barSprites[i] = std::make_shared<SpriteBarEntry>(i);
        _sprites.push_back(_barSprites[i]);
    }
    _laneSprites.resize(chart::LANE_COUNT);
    updateDstOpt();
    loadCSV(p, headerOnly);

    startSpriteVideoPlayback();
}

SkinLR2::~SkinLR2()
{
    stopSpriteVideoPlayback();
}

void SkinLR2::loadCSV(Path p, bool headerOnly)
{
    if (filePath.empty())
        filePath = p;

    auto srcLineNumberParent = csvLineNumber;
    csvLineNumber = 0;

    p = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), p);

    std::ifstream ifsFile(p, std::ios::binary);
    if (!ifsFile.is_open())
    {
        LOG_ERROR << "[Skin] File Not Found: " << std::filesystem::absolute(p).u8string();
        csvLineNumber = srcLineNumberParent;
        return;
    }

    // copy the whole file into ram, once for all
    std::stringstream csvFile;
    csvFile << ifsFile.rdbuf();
    csvFile.sync();
    ifsFile.close();

    auto encoding = getFileEncoding(csvFile);

    LOG_INFO << "[Skin] File (" << getFileEncodingName(encoding) << "): " << p.u8string();

    bool haveEndOfHeader = false;
    while (!csvFile.eof())
    {
        std::string raw;
        std::getline(csvFile, raw);
        ++csvLineNumber;

        // convert codepage
        raw = to_utf8(raw, encoding);

        auto tokens = csvLineTokenize(raw);
        if (tokens.empty()) continue;

        if (parseHeader(tokens) == -1)
        {
            // #ENDOFHEADER
            haveEndOfHeader = true;
            break;
        }
    }
    LOG_DEBUG << "[Skin] File: " << p.u8string() << "(Line " << csvLineNumber << "): Header loading finished";

    if (!headerOnly)
    {
        if (!haveEndOfHeader && csvFile.eof())
        {
            // reset position to head
            csvFile.clear();
            csvFile.seekg(0);
            csvLineNumber = 0;
        }

        // load skin customization from profile
        Path pCustomize = ConfigMgr::Profile()->getPath() / "customize" / SceneCustomize::getConfigFileName(getFilePath());
        try
        {
            std::map<size_t, StringContent> opDstMap;
            std::map<StringContent, StringContent> opFileMap;
            for (const auto& node : YAML::LoadFile(pCustomize.u8string()))
            {
                auto key = node.first.as<std::string>();
                if (key.substr(0, 4) == "OPT_")
                {
                    size_t dst_op = std::strtoul(key.substr(4).c_str(), nullptr, 10);
                    opDstMap[dst_op] = node.second.as<std::string>();
                }
                else if (key.substr(0, 5) == "FILE_")
                {
                    opFileMap[key.substr(5)] = node.second.as<std::string>();
                }
            }
            for (auto& itOp : customize)
            {
                for (auto& itDst : opDstMap)
                {
                    if (itDst.first == itOp.dst_op)
                    {
                        if (const auto itEntry = std::find(itOp.label.begin(), itOp.label.end(), itDst.second); itEntry != itOp.label.end())
                        {
                            itOp.value = std::distance(itOp.label.begin(), itEntry);
                        }
                    }
                }
                for (auto& itFile : opFileMap)
                {
                    if (itOp.title == itFile.first && itOp.dst_op == 0)
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
        for (auto c : customize)
        {
            if (c.dst_op != 0)
            {
                //data().setDstOption(static_cast<dst_option>(c.dst_op + c.value), true);
                for (size_t i = 0; i < c.label.size(); ++i)
                {
                    setCustomDstOpt(c.dst_op, i, false);
                }
                setCustomDstOpt(c.dst_op, c.value, true);
            }
        }

        // Add extra textures

        while (!csvFile.eof())
        {
            std::string raw;
            std::getline(csvFile, raw);
            ++csvLineNumber;

            raw = to_utf8(raw, encoding);

            auto tokens = csvLineTokenize(raw);
            if (tokens.empty()) continue;

            if (strEqual(*tokens.begin(), "#IF", true))
                IF(tokens, csvFile);
            else
                parseBody(tokens);
        }

        // set barcenter
        if (barCenter < _barSprites.size())
        {
            _barSprites[barCenter]->drawFlash = true;

            if (gResetSelectCursor)
            {
                gResetSelectCursor = false;
                gSelectContext.cursor = barCenter;
            }
        }

        // set note area height
        for (auto& s : _sprites)
        {
            auto pS = std::dynamic_pointer_cast<SpriteLaneVertical>(s);
            if (pS != nullptr)
            {
                pS->setHeight(_noteAreaHeight);
            }
        }
    }


    LOG_DEBUG << "[Skin] File: " << p.u8string() << "(Line " << csvLineNumber << "): Body loading finished";
    _loaded = true;

    /*
    loadImages();
    convertImageToTexture();
    for (auto& e : elements)
    {
        if (e && e->type() != elementType::TEXT)
        {
            createSprite(*e);
        }
    }
    */

    csvLineNumber = srcLineNumberParent;
}


//////////////////////////////////////////////////

void SkinLR2::update()
{
    // update sprites
    vSkin::update();

    // update op
    updateDstOpt();

    Time t;

    // update turntables
    {
        int ttAngle1P = gNumbers.get(eNumber::_ANGLE_TT_1P);
        int ttAngle2P = gNumbers.get(eNumber::_ANGLE_TT_2P);

#ifndef _DEBUG
        std::for_each(std::execution::par_unseq, drawQueue.begin(), drawQueue.end(), [ttAngle1P, ttAngle2P](auto& e)
#else
        for (auto& e : drawQueue)
#endif
        {
            e.ps->setHide(!(getDstOpt(e.op1) && getDstOpt(e.op2) && getDstOpt(e.op3)));
            for (auto op : e.opEx)
            {
                if (!getDstOpt(op))
                    e.ps->setHide(true);
            }
            switch (e.op4)
            {
            case 1: e.ps->_current.angle += ttAngle1P; break;
            case 2: e.ps->_current.angle += ttAngle2P; break;
            default: break;
            }

        }
#ifndef _DEBUG
        );
#endif
    }

    // update nowjudge/nowcombo
    for (size_t i = 0; i < 6; ++i)
    {
        if (gSprites[i] && gSprites[i + 6] && gSprites[i]->_draw && gSprites[i + 6]->_draw)
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 6]);
            Rect diff{ 0,0,0,0 };
            //diff.x = int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            diff.x += judge->_current.rect.x;
            diff.y += judge->_current.rect.y;
            if (!noshiftJudge1P[i])
            {
                judge->_current.rect.x -= int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            }
            for (auto& d : combo->_rects)
            {
                d.x += diff.x;
                d.y += diff.y;
            }
        }
        if (gSprites[i + 12] && gSprites[i + 18] && gSprites[i + 12]->_draw && gSprites[i + 18]->_draw)
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i + 12]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 18]);
            Rect diff{ 0,0,0,0 };
            //diff.x = int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            diff.x += judge->_current.rect.x;
            diff.y += judge->_current.rect.y;
            if (!noshiftJudge2P[i])
            {
                judge->_current.rect.x -= int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            }
            for (auto& d : combo->_rects)
            {
                d.x += diff.x;
                d.y += diff.y;
            }
        }
    }

    // update songlist bar
    {
        // read lock
        std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);
        for (auto& s : _barSprites) s->update(t);

        if (hasBarAnimOrigin)
        {
            auto tMove = gTimers.get(eTimer::LIST_MOVE);
            if (t.norm() - tMove < barAnimTimeLength)
            {
                setListStopTimer = true;
                for (size_t i = 1; i + 1 < _barSprites.size(); ++i)
                {
                    if (!_barSpriteAdded[i]) continue;

                    double factor = 1.0 - (t.norm() - tMove) / double(barAnimTimeLength);

                    auto& rectStored = _barAnimOrigin[i];
                    auto& rectSprite = _barSprites[i]->_current.rect;
                    Rect dr{
                        static_cast<int>(std::round((rectStored.x - rectSprite.x) * factor)),
                        static_cast<int>(std::round((rectStored.y - rectSprite.y) * factor)),
                        0, 0
                    };
                    _barSprites[i]->setRectOffset(dr);

                }
            }
            else
            {
                if (setListStopTimer)
                {
                    setListStopTimer = false;
                    gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());
                }
            }
        }
    }
}

void SkinLR2::reset_bar_animation()
{
    hasBarAnimOrigin = false;
    _barAnimOrigin.fill(Rect(0, 0, 0, 0));
}

void SkinLR2::start_bar_animation(int direction)
{
    barAnimTimeLength = gSelectContext.scrollTime;
    for (size_t i = 0; i < BAR_ENTRY_SPRITE_COUNT; ++i)
    {
        int di = direction + i;
        if (di < 0 || di >= BAR_ENTRY_SPRITE_COUNT || !_barSpriteAdded[di])
        {
            _barAnimOrigin[i] = _barSprites[i]->_current.rect;
        }
        else
        {
            int j = di % BAR_ENTRY_SPRITE_COUNT;
            _barAnimOrigin[i] = _barSprites[j]->_current.rect;
        }
    }
    hasBarAnimOrigin = true;
}

void SkinLR2::draw() const
{
    for (auto& e : drawQueue)
    {
        e.ps->draw();
    }
    //for (auto& c : _csvIncluded)
    //{
    //    c.draw();
    //}
}

size_t SkinLR2::getCustomizeOptionCount() const
{
    return customize.size();
}

vSkin::CustomizeOption SkinLR2::getCustomizeOptionInfo(size_t idx) const
{
    CustomizeOption ret;
    const auto& op = customize[idx];

    ret.id = op.dst_op;
    switch (op.type)
    {
    case Customize::_Type::OPT:
        ret.internalName = "OPT_";
        ret.internalName += std::to_string(op.dst_op);
        ret.displayName = op.title;
        ret.entries = op.label;
        ret.defaultEntry = 0;
        break;

    case Customize::_Type::FILE:
        ret.internalName = "FILE_";
        ret.internalName += op.title;
        ret.displayName = op.title;
        for (size_t i = 0; i < op.pathList.size(); ++i)
            ret.entries.push_back(op.pathList[i].filename().stem().u8string());
        ret.defaultEntry = op.defIdx;
        break;
    }
    return ret;
}


StringContent SkinLR2::getName() const
{
    return info.name;
}

StringContent SkinLR2::getMaker() const
{
    return info.maker;
}

StringPath SkinLR2::getFilePath() const
{
    return filePath.is_absolute() ? filePath : filePath.relative_path();
}
