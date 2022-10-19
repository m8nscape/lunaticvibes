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
#include "game/runtime/state.h"
#include "game/graphics/video.h"
#include "game/scene/scene_context.h"
#include "skin_lr2_converters.h"
#include "config/config_mgr.h"
#include "game/scene/scene_customize.h"
#include "game/graphics/dxa.h"
#include "game/graphics/video.h"
#include "re2/re2.h"

#include <boost/algorithm/string.hpp>

#ifdef _WIN32
// For GetWindowsDirectory
#define WIN32_LEAN_AND_MEAN
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

static bool flipSideFlag = false;

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

            if (flipSideFlag)
            {
                switch (timer)
                {
                case 42:    timer = 43; break;
                case 43:    timer = 42; break;
                case 44:    timer = 45; break;
                case 45:    timer = 44; break;
                case 46:    timer = 47; break;
                case 47:    timer = 46; break;
                case 48:    timer = 49; break;
                case 49:    timer = 48; break;
                case 143:   timer = 144; break;
                case 144:   timer = 143; break;
                default:
                    if (50 <= timer && timer <= 59 ||
                        70 <= timer && timer <= 79 ||
                        100 <= timer && timer <= 109 ||
                        120 <= timer && timer <= 129)
                        timer += 10;
                    else if (60 <= timer && timer <= 69 ||
                        80 <= timer && timer <= 89 ||
                        110 <= timer && timer <= 119 ||
                        130 <= timer && timer <= 139)
                        timer -= 10;
                    break;
                }
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

            if (flipSideFlag)
            {
                if      (100 <= num && num <= 119) num += 20;
                else if (302 <= num && num <= 306) num += 40;
                else if (320 <= num && num <= 329) num += 10;
                else if (360 <= num && num <= 369) num += 10;
                else if (120 <= num && num <= 139) num -= 20;
                else if (342 <= num && num <= 346) num -= 40;
                else if (330 <= num && num <= 339) num -= 10;
                else if (370 <= num && num <= 379) num -= 10;
                else
                {
                    switch (num)
                    {
                    case 10:  num = 11; break;
                    case 11:  num = 10; break;
                    case 14:  num = 15; break;
                    case 15:  num = 14; break;
                    case 201: num = 213; break;
                    case 210: num = 211; break;
                    case 211: num = 210; break;
                    case 212: num = 371; break;
                    case 213: num = 213; break;
                    case 214: num = 372; break;
                    }
                }
            }
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

            if (flipSideFlag)
            {
                switch (type)
                {
                case 2: type = 3; break;
                case 3: type = 2; break;
                case 4: type = 5; break;
                case 5: type = 4; break;
                case 8: type = 9; break;
                case 9: type = 8; break;
                }
            }
        }
    };

    struct s_bargraph : s_basic
    {
        int type = 0;
        int muki = 0;
        s_bargraph(const Tokens& tokens, size_t csvLineNumber = 0) : s_basic(tokens, csvLineNumber)
        {
            convertLine(tokens, (int*)this, &type - &_null, 2);
            
            if (flipSideFlag)
            {
                if      (10 <= type && type <= 11) type += 4;
                else if (20 <= type && type <= 29) type += 10;
                else if (14 <= type && type <= 15) type -= 4;
                else if (30 <= type && type <= 39) type -= 10;
            }
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

            if (flipSideFlag)
            {
                switch (st)
                {
                case 1:  st = 2; break;
                case 2:  st = 1; break;
                case 63: st = 64; break;
                case 64: st = 63; break;
                case 65: st = 66; break;
                case 66: st = 65; break;
                case 67: st = 68; break;
                case 68: st = 67; break;
                case 84: st = 85; break;
                case 85: st = 84; break;
                }
            }
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

            // DO not flip index here; there are two separate definitions called SRC_NOWJUDGE_1P / SRC_NOWJUDGE_2P
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

            // DO not flip index here; there are two separate definitions called SRC_NOWCOMBO_1P / SRC_NOWCOMBO_2P
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

            switch (_null)
            {
            case 0: _null = lr2skin::flipSideFlag ? 1 : 0; break;
            case 1: _null = lr2skin::flipSideFlag ? 0 : 1; break;
            }
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

            if (flipSideFlag)
            {
                switch (timer)
                {
                case 42:    timer = 43; break;
                case 43:    timer = 42; break;
                case 44:    timer = 45; break;
                case 45:    timer = 44; break;
                case 46:    timer = 47; break;
                case 47:    timer = 46; break;
                case 48:    timer = 49; break;
                case 49:    timer = 48; break;
                case 143:   timer = 144; break;
                case 144:   timer = 143; break;
                default:
                    if (50 <= timer && timer <= 59 ||
                        70 <= timer && timer <= 79 ||
                        100 <= timer && timer <= 109 ||
                        120 <= timer && timer <= 129)
                        timer += 10;
                    else if (60 <= timer && timer <= 69 ||
                        80 <= timer && timer <= 89 ||
                        110 <= timer && timer <= 119 ||
                        130 <= timer && timer <= 139)
                        timer -= 10;
                    break;
                }
            }
        }
    };

    BlendMode convertBlend(int blend)
    {
        static const std::map<int, BlendMode> blendMap =
        {
            {0, BlendMode::NONE},
            {1, BlendMode::ALPHA},
            {2, BlendMode::ADD},
            {3, BlendMode::SUBTRACT},
            {4, BlendMode::MOD},
            {6, BlendMode::ADD},    // LR2: XOR but implemented as ADD
            {9, BlendMode::MULTIPLY_INVERTED_BACKGROUND},
            {10, BlendMode::INVERT},
            {11, BlendMode::MULTIPLY_WITH_ALPHA},
        };
        return (blendMap.find(blend) != blendMap.end()) ? blendMap.at(blend) : BlendMode::ALPHA;
    }
}

std::map<std::string, pTexture> SkinLR2::LR2SkinImageCache;

std::map<std::string, Path> SkinLR2::LR2SkinFontPathCache;
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
        res.push_back(Token(token));
        idx = pos + 1;
    } while (pos != linecsv.npos);
    return res;
}

Tokens csvLineTokenize(const std::string& raw)
{
    if (size_t commentIdx = raw.find("//"); commentIdx != raw.npos)
    {
        return csvLineTokenize(raw.substr(0, commentIdx));
    }

    if (raw.find('\\') == raw.npos)
    {
        return csvLineTokenizeSimple(raw);
    }

    StringContentView linecsv = csvLineNormalize(raw);
    if (linecsv.empty()) return {};

    Tokens res;
    res.reserve(32);

    auto lineBuf = re2::StringPiece(linecsv.data(), linecsv.length());
    static const LazyRE2 re{ R"(((?:(?:\\,)|[^,])*?)(?:,|$))" };
    std::string token;
    while (!lineBuf.empty() && RE2::Consume(&lineBuf, *re, &token))
    {
        res.push_back(token);
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

Path SkinLR2::getCustomizePath(StringContentView input)
{
    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), input));
    StringPath pathStr = path.native();
    std::string pathU8Str = path.u8string();

    if (pathStr.find("*"_p) != pathStr.npos)
    {
        // Check if the wildcard path is specified by custom settings
        std::srand(std::time(NULL));
        for (size_t idx = 0; idx < customize.size(); ++idx)
        {
            const auto& cf = customize[idx];
            if (cf.type == Customize::_Type::FILE && cf.filepath == pathU8Str.substr(0, cf.filepath.length()))
            {
                int value = (cf.pathList[cf.value] == "RANDOM") ? customizeRandom[idx] : cf.value;

                Path pathFile = cf.pathList[value];
                if (cf.filepath.length() < pathU8Str.length())
                    pathFile /= PathFromUTF8(pathU8Str.substr(cf.filepath.length() + 1));

                return pathFile;
            }
        }

        // Or, randomly choose a file
        auto ls = findFiles(path);
        if (ls.empty())
        {
            return Path();
        }
        else
        {
            size_t ranidx = std::rand() % ls.size();
            return ls[ranidx];
        }
    }

    // Normal path
    return path;
}


int SkinLR2::IMAGE()
{
    if (!strEqual(parseKeyBuf, "#IMAGE", true)) return 0;

    if (strEqual(parseParamBuf[0], "CONTINUE", true))
    {
        // already referenced inside constructor; create a blank texture if not exist
        std::string textureMapKey = std::to_string(imageCount);
        if (_textureNameMap.find(textureMapKey) == _textureNameMap.end())
        {
            _textureNameMap[textureMapKey] = std::make_shared<Texture>(nullptr, 0, 0);
        }
        ++imageCount;
        return 1;
    }
    else
    {
        Path pathFile = getCustomizePath(parseParamBuf[0]);
        std::string textureMapKey = std::to_string(imageCount);

        if (video_file_extensions.find(toLower(pathFile.extension().u8string())) != video_file_extensions.end())
        {
#ifndef VIDEO_DISABLED
            _vidNameMap[textureMapKey] = std::make_shared<sVideo>(pathFile, 1.0, true);
            _textureNameMap[textureMapKey] = _textureNameMap["White"];
#else
            _textureNameMap[textureMapKey] = _textureNameMap["Black"];
#endif
        }
        else
        {
            Image img = Image(pathFile.u8string().c_str());
            if (info.hasTransparentColor)
                img.setTransparentColorRGB(info.transparentColor);
            _textureNameMap[textureMapKey] = std::make_shared<Texture>(img);
        }

        LR2SkinImageCache[textureMapKey] = _textureNameMap[textureMapKey];
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added IMAGE[" << imageCount << "]: " << pathFile;

        ++imageCount;
        return 1;
    }
    return 0;
}

int SkinLR2::LR2FONT()
{
    if (!strEqual(parseKeyBuf, "#LR2FONT", true)) return 0;

    if (loadMode >= 1)
    {
        std::string fontNameKey = std::to_string(LR2FontNameMap.size());
        LR2FontNameMap[fontNameKey] = nullptr;
        return 1;
    }

    if (strEqual(parseParamBuf[0], "CONTINUE", true))
    {
        // already referenced inside constructor; create a blank texture if not exist
        std::string fontNameKey = std::to_string(LR2FontNameMap.size());
        if (LR2SkinFontPathCache.find(fontNameKey) != LR2SkinFontPathCache.end())
        {
            Path path = LR2SkinFontPathCache[fontNameKey];
            LR2FontNameMap[fontNameKey] = LR2FontCache[path];
        }
        else
        {
            LR2FontNameMap[fontNameKey] = nullptr;
        }
        return 1;
    }
    else
    {
        Path path = getCustomizePath(parseParamBuf[0]);
        path = std::filesystem::absolute(path);
        std::string fontNameKey = std::to_string(LR2FontNameMap.size());

        if (LR2FontCache.find(path) != LR2FontCache.end())
        {
            LR2FontNameMap[fontNameKey] = LR2FontCache[path];
            LR2SkinFontPathCache[fontNameKey] = path;
            return 1;
        }

        findAndExtractDXA(path);

        if (!fs::is_regular_file(path))
        {
            LR2FontNameMap[fontNameKey] = nullptr;
            LOG_WARNING << "[Skin] " << csvLineNumber << ": LR2FONT file not found: " << path.u8string();
            return 1;
        }

        std::ifstream ifsFile(path, std::ios::binary);
        if (ifsFile.fail())
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": LR2FONT file open failed: " << path.u8string();
            return 1;
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
            std::string rawUTF8 = to_utf8(raw, encoding);

            auto tokens = csvLineTokenize(rawUTF8);
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
                findAndExtractDXA(p);
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

                c_utf32 = to_utf32(s_sjis, eFileEncoding::SHIFT_JIS)[0];

                Rect r;
                r.x = toInt(tokens[3]);
                r.y = toInt(tokens[4]);
                r.w = toInt(tokens[5]);
                r.h = toInt(tokens[6]);

                if (c_utf32 == U' ')
                {
                    // ID = space, LR2 displays a blank texture instead of actually texture file for spaces
                    pf->R[c_utf32] = { size_t(-1), r };
                }
                else
                {
                    pf->R[c_utf32] = { pf->T_id.at(imgId), r };
                }
            }
        }

        LR2FontCache[path] = pf;
        LR2FontNameMap[fontNameKey] = pf;
        LR2SkinFontPathCache[fontNameKey] = path;
        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Added LR2FONT[" << fontNameKey << "]: " << path.u8string();
        return 1;
    }
}

int SkinLR2::SYSTEMFONT()
{
    // Could not get system font file path in a reliable way while cross-platforming..
    if (strEqual(parseKeyBuf, "#FONT", true))
    {
        if (loadMode >= 1)
        {
            size_t idx = _fontNameMap.size();
            _fontNameMap[std::to_string(idx)] = nullptr;
            return 1;
        }

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
        Path path = getCustomizePath(parseParamBuf[0]);

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

        switch (info.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY5_2:
        case eMode::PLAY7:
        case eMode::PLAY7_2:
        case eMode::PLAY9:
        case eMode::PLAY10:
        case eMode::PLAY14:
            lr2skin::flipSideFlag = flipSide;
            break;
        }

        return 3;
    }
    if (strEqual(parseKeyBuf, "#FLIPRESULT", true))
    {
        flipResult = true;

        if (info.mode == eMode::RESULT)
        {
            lr2skin::flipSideFlag = flipResult && !disableFlipResult;
        }

        return 4;
    }
    if (strEqual(parseKeyBuf, "#DISABLEFLIP", true))
    {
        disableFlipResult = true;

        if (info.mode == eMode::RESULT)
        {
            lr2skin::flipSideFlag = flipResult && !disableFlipResult;
        }

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

    // partial match is enough
    DefType type = DefType::UNDEF;
    for (auto& [typeName, defType] : defTypeName)
    {
        if (name.substr(0, typeName.length()) == typeName)
        {
            type = defType;
            break;
        }
    }

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
        if (gr == 105)
        {
            textureBuf = getTextureCustomizeThumbnail();
            videoBuf = nullptr;
            useVideo = false;
        }
        else
        {
            switch (gr)
            {
            case 100: gr_key = "STAGEFILE"; break;
            case 101: gr_key = "BACKBMP"; break;
            case 102: gr_key = "BANNER"; break;
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
                // textureBuf = _textureNameMap["Error"];
                textureBuf = std::make_shared<Texture>(nullptr, 0, 0);
                videoBuf = nullptr;
                useVideo = false;
            }
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
        case DefType::NOWJUDGE_1P:    lr2skin::flipSideFlag ? SRC_NOWJUDGE2() : SRC_NOWJUDGE1(); break;
        case DefType::NOWJUDGE_2P:    lr2skin::flipSideFlag ? SRC_NOWJUDGE1() : SRC_NOWJUDGE2(); break;
        case DefType::NOWCOMBO_1P:    lr2skin::flipSideFlag ? SRC_NOWCOMBO2() : SRC_NOWCOMBO1(); break;
        case DefType::NOWCOMBO_2P:    lr2skin::flipSideFlag ? SRC_NOWCOMBO1() : SRC_NOWCOMBO2(); break;
        case DefType::BGA:            SRC_BGA();                 break;
        case DefType::MOUSECURSOR:    SRC_MOUSECURSOR();         break;
        case DefType::GAUGECHART_1P:  SRC_GAUGECHART(lr2skin::flipSideFlag ? 1 : 0); break;
        case DefType::GAUGECHART_2P:  SRC_GAUGECHART(lr2skin::flipSideFlag ? 0 : 1); break;
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
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x));
    }

    _sprites.back()->setSrcLine(csvLineNumber);
    
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NUMBER()
{
    lr2skin::s_number d(parseParamBuf, csvLineNumber);

    IndexNumber iNum = lr2skin::num(d.num);

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
        textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (IndexTimer)d.timer, f));
    _sprites.back()->setSrcLine(csvLineNumber);

    switch (iNum)
    {
    case IndexNumber::LR2IR_REPLACE_PLAY_1P_JUDGE_TIME_ERROR_MS:
    case IndexNumber::LR2IR_REPLACE_PLAY_2P_JUDGE_TIME_ERROR_MS:
    case IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_SLOW:
    case IndexNumber::LR2IR_REPLACE_PLAY_2P_FAST_SLOW:
    case IndexNumber::LR2IR_REPLACE_PLAY_1P_FAST_COUNT:
    case IndexNumber::LR2IR_REPLACE_PLAY_1P_SLOW_COUNT:
    case IndexNumber::PLAY_1P_FAST_COUNT:
    case IndexNumber::PLAY_1P_SLOW_COUNT:
    case IndexNumber::PLAY_2P_FAST_COUNT:
    case IndexNumber::PLAY_2P_SLOW_COUNT:
    case IndexNumber::PLAY_1P_JUDGE_TIME_ERROR_MS:
    case IndexNumber::PLAY_2P_JUDGE_TIME_ERROR_MS:
        isSupportFastSlow = true;
        break;

    case IndexNumber::GREEN_NUMBER_1P:
    case IndexNumber::GREEN_NUMBER_2P:
    case IndexNumber::GREEN_NUMBER_MAXBPM_1P:
    case IndexNumber::GREEN_NUMBER_MAXBPM_2P:
    case IndexNumber::GREEN_NUMBER_MINBPM_1P:
    case IndexNumber::GREEN_NUMBER_MINBPM_2P:
        isSupportGreenNumber = true;
        break;
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_SLIDER()
{
    lr2skin::s_slider d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteSlider>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (SliderDirection)d.muki, d.range, 
        d.disable ? lr2skin::slider::getSliderCallback(-1) : lr2skin::slider::getSliderCallback(d.type),
        d.div_y*d.div_x, d.cycle, (IndexSlider)d.type, (IndexTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    switch ((IndexSlider)d.type)
    {
    case IndexSlider::SUD_1P:
        spriteLanecoverTop1P = _sprites.back();
        break;
    case IndexSlider::SUD_2P:
        spriteLanecoverTop1P = _sprites.back();
        break;

    case IndexSlider::HID_1P:
    case IndexSlider::HID_2P:
        isSupportLift = true;
        break;
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BARGRAPH()
{
    lr2skin::s_bargraph d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteBargraph>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (BargraphDirection)d.muki, d.div_y*d.div_x, d.cycle, (IndexBargraph)d.type, (IndexTimer)d.timer, d.div_y, d.div_x));
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
                textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, lr2skin::button::getButtonCallback(d.type), d.panel, d.plusonly, IndexTimer::SCENE_START, d.div_y, d.div_x, false);
        }
        else
        {
            s = std::make_shared<SpriteOption>(
                textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, IndexTimer::SCENE_START, d.div_y, d.div_x, false);
        }
        IndexSwitch sw;
        IndexOption op;
        if (lr2skin::buttonSw(d.type, sw))
        {
            if (sw == IndexSwitch::_TRUE)
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

            if (info.mode == eMode::MUSIC_SELECT)
            {
                if (op == IndexOption::PLAY_GAUGE_TYPE_1P || op == IndexOption::PLAY_GAUGE_TYPE_2P)
                {
                    if (d.div_x * d.div_y >= 6)
                        isSupportExHardAndAssistEasy = true;
                }
                if (op == IndexOption::PLAY_LANE_EFFECT_TYPE_1P || op == IndexOption::PLAY_LANE_EFFECT_TYPE_2P)
                {
                    if (d.div_x * d.div_y >= 6)
                        isSupportLift = true;
                }
            }
        }
        _sprites.push_back(s);
        _sprites.back()->setSrcLine(csvLineNumber);
    }
    else
    {
        // deal as IndexSwitch::_FALSE
        auto s = std::make_shared<SpriteOption>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, IndexTimer::SCENE_START, d.div_y, d.div_x, false);
        s->setInd(SpriteOption::opType::SWITCH, (unsigned)IndexSwitch::_FALSE);
        _sprites.push_back(s);
        _sprites.back()->setSrcLine(csvLineNumber);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_ONMOUSE()
{
    lr2skin::s_onmouse d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteOnMouse>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, d.panel, Rect(d.x2, d.y2, d.w2, d.h2), (IndexTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_MOUSECURSOR()
{
    lr2skin::s_mousecursor d(parseParamBuf, csvLineNumber);

    _sprites.push_back(std::make_shared<SpriteCursor>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x));
    _sprites.back()->setSrcLine(csvLineNumber);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_TEXT()
{
    if (loadMode >= 1) return ParseRet::OK;

    lr2skin::s_text d(parseParamBuf);

    auto font = std::to_string(d.font);
    if (LR2FontNameMap.find(font) != LR2FontNameMap.end() && LR2FontNameMap[font] != nullptr)
    {
        auto& pf = LR2FontNameMap[font];
        auto ps = std::make_shared<SpriteImageText>(pf->T_texture, &pf->R, (IndexText)d.st, (TextAlign)d.align, pf->S, pf->M);
        if (d.edit) ps->setEditable(true);
        _sprites.push_back(ps);
    }
    else
    {
        auto ps = std::make_shared<SpriteText>(_fontNameMap[std::to_string(d.font)], (IndexText)d.st, (TextAlign)d.align);
        if (d.edit) ps->setEditable(true);
        _sprites.push_back(ps);
    }

    _sprites.back()->setSrcLine(csvLineNumber);

    switch (d.st)
    {
    case 84:
    case 85:
        isSupportLift = true;
        break;
    }

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
        player == 0 ? PLAYER_SLOT_PLAYER : PLAYER_SLOT_TARGET,
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
    case 1: type = LineType::SCORE_MYBEST; color = { 20, 255, 20, 255 };   break;
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
    if (lr2skin::flipSideFlag)
    {
        d._null = (d._null == 0) ? 1 : 0;
    }

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
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
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
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
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

    IndexNumber iNum = lr2skin::num(d.num);

    // get NumType from div_x, div_y
    unsigned f = d.div_y * d.div_x;
    if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
    else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
    else if (f % NumberType::NUM_TYPE_FULL == 0) f = f / NumberType::NUM_TYPE_FULL;
    else f = 0;

    gSprites[idx] = std::make_shared<SpriteNumber>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (IndexTimer)d.timer, f);
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
    IndexNumber en = d._null == 0 ? IndexNumber::PLAY_1P_GROOVEGAUGE : IndexNumber::PLAY_2P_GROOVEGAUGE;

    gSprites[idx] = std::make_shared<SpriteGaugeGrid>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x / 4, d.cycle, d.add_x, d.add_y, 0, 100, 50,
        (IndexTimer)d.timer, en, d.div_y, d.div_x);
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
    static StringContent eNumBuf = std::to_string((int)IndexNumber::_DISP_NOWCOMBO_1P);
    parseParamBuf[10] = StringContentView(eNumBuf);
    alignNowCombo1P[bufJudge1PSlot] = toInt(parseParamBuf[11]);
    parseParamBuf[11] = "1";

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
    static StringContent eNumBuf = std::to_string((int)IndexNumber::_DISP_NOWCOMBO_2P);
    parseParamBuf[10] = StringContentView(eNumBuf);
    alignNowCombo2P[bufJudge2PSlot] = toInt(parseParamBuf[11]);
    parseParamBuf[11] = "1";

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

chart::NoteLaneIndex NoteIdxToLane(eMode gamemode, int idx)
{
    assert(idx < 20);

    using namespace chart;
    switch (gamemode)
    {
    case eMode::PLAY5:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, K1, K2, K3, K4, K5, _, _, _, _,
            _, _, _, _, _, _, _, _, _, _,
        };
        return lane[idx];
    }
    case eMode::PLAY7:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, K1, K2, K3, K4, K5, K6, K7, _, _,
            _, _, _, _, _, _, _, _, _, _,
        };
        return lane[idx];
    }
    case eMode::PLAY9:
    {
        static const NoteLaneIndex lane[] =
        {
            _, K1, K2, K3, K4, K5, K6, K7, K8, K9,
            _, _, _, _, _, _, _, _, _, _,
        };
        return lane[idx];
    }
    case eMode::PLAY5_2:
    case eMode::PLAY10:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, K1, K2, K3, K4, K5, _, _, _, _,
            Sc2, K6, K7, K8, K9, K10, _, _, _, _,
        };
        return lane[idx];
    }
    case eMode::PLAY7_2:
    case eMode::PLAY14:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, K1, K2, K3, K4, K5, K6, K7, _, _,
            Sc2, K8, K9, K10, K11, K12, K13, K14, _, _,
        };
        return lane[idx];
    }
    }
    return _;
}

ParseRet SkinLR2::SRC_NOTE(DefType type)
{
    using namespace chart;

    // load raw into data struct
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);
    if (lr2skin::flipSideFlag)
    {
        if (type == DefType::LINE)
        {
            d._null = (d._null == 0) ? 1 : 0;
        }
    }

    IndexTimer iTimer = lr2skin::timer(d.timer);

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
    bool autoNotes = false;

    // SRC_NOTE
    switch (type)
    {
    case DefType::LINE:
        cat = NoteLaneCategory::EXTRA;
        idx = (NoteLaneIndex)(d._null == 0 ? EXTRA_BARLINE_1P : EXTRA_BARLINE_2P);
        break;

    case DefType::NOTE:
        cat = NoteLaneCategory::Note;
        idx = NoteIdxToLane(info.mode, d._null);
        break;
    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
        cat = NoteLaneCategory::LN;
        idx = NoteIdxToLane(info.mode, d._null);
        break;
    case DefType::MINE:
        cat = NoteLaneCategory::Mine;
        idx = NoteIdxToLane(info.mode, d._null);
        break;

    case DefType::AUTO_NOTE:
        cat = NoteLaneCategory::Note;
        idx = NoteIdxToLane(info.mode, d._null);
        autoNotes = true;
        break;
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:
        cat = NoteLaneCategory::LN;
        idx = NoteIdxToLane(info.mode, d._null);
        autoNotes = true;
        break;
    case DefType::AUTO_MINE:
        cat = NoteLaneCategory::Mine;
        idx = NoteIdxToLane(info.mode, d._null);
        autoNotes = true;
        break;

    default:
        return ParseRet::SRC_DEF_INVALID;
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
    {
        _sprites.push_back(std::make_shared<SpriteLaneVertical>(
            _textureNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x, false, d._null == 0 ? 0 : 1));

        auto& ls = _laneSprites[i].first;

        ls = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
        ls->setLane(cat, idx);
        ls->pNote->appendKeyFrame({ 0, {Rect(),
            RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0.0 } });
        ls->pNote->setLoopTime(0);
        break;
    }

    case DefType::NOTE:
    case DefType::MINE:
    case DefType::AUTO_NOTE:
    case DefType::AUTO_MINE:
    {
        _sprites.push_back(std::make_shared<SpriteLaneVertical>(
            _textureNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x, false, !!(d._null >= 10), autoNotes));

        std::shared_ptr<SpriteLaneVertical> ls = nullptr;
        if (!autoNotes)
        {
            _laneSprites[i].first = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
            ls = _laneSprites[i].first;
        }
        else
        {
            _laneSprites[i].second = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
            ls = _laneSprites[i].second;
        }
        ls->setLane(cat, idx);
        ls->pNote->appendKeyFrame({ 0, {Rect(),
            RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0.0 } });
        ls->pNote->setLoopTime(0);
        break;
    }

    case DefType::LN_END:
    case DefType::LN_BODY:
    case DefType::LN_START:
    case DefType::AUTO_LN_END:
    case DefType::AUTO_LN_BODY:
    case DefType::AUTO_LN_START:
    {
        std::shared_ptr<SpriteLaneVerticalLN> p = nullptr;
        if (!autoNotes)
        {
            if (_laneSprites[i].first == nullptr)
            {
                _sprites.push_back(std::make_shared<SpriteLaneVerticalLN>(!!(d._null >= 10), false));
                _laneSprites[i].first = std::static_pointer_cast<SpriteLaneVerticalLN>(_sprites.back());
                _laneSprites[i].first->setLane(cat, idx);
            }
            p = std::static_pointer_cast<SpriteLaneVerticalLN>(_laneSprites[i].first);
        }
        else
        {
            if (_laneSprites[i].second == nullptr)
            {
                _sprites.push_back(std::make_shared<SpriteLaneVerticalLN>(!!(d._null >= 10), true));
                _laneSprites[i].second = std::static_pointer_cast<SpriteLaneVerticalLN>(_sprites.back());
                _laneSprites[i].second->setLane(cat, idx);
            }
            p = std::static_pointer_cast<SpriteLaneVerticalLN>(_laneSprites[i].second);
        }
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
    default: return ParseRet::PARAM_INVALID;
    }

    for (auto& bar : _barSprites)
    {
        bar->setBody(type,
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);

        if (type == BarType::SONG)
            bar->setBody(BarType::NEW_SONG,
                textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_FLASH()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    for (auto& bar : _barSprites)
    {
        bar->setFlash(textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
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
            textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, (IndexTimer)d.timer, f);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_LAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setLamp(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
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
            bar->setTitle(type, pf->T_texture, &pf->R, (TextAlign)d.align, pf->S, pf->M);
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
        bar->setRank(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarRivalType type = BarRivalType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalWinLose(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_MYLAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampSelf(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_RIVALLAMP()
{
    lr2skin::s_basic d(parseParamBuf, csvLineNumber);

    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampRival(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (IndexTimer)d.timer, d.div_y, d.div_x);
    }

    return ParseRet::OK;
}


#pragma endregion

bool SkinLR2::DST()
{
    if (!strEqual(parseKeyBuf.substr(0, 5), "#DST_", true))
        return false;

    auto name = toUpper(parseKeyBuf.substr(5));

    // partial match is enough
    DefType type = DefType::UNDEF;
    for (auto& [typeName, defType] : defTypeName)
    {
        if (name.substr(0, typeName.length()) == typeName)
        {
            type = defType;
            break;
        }
    }

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

    case DefType::TEXT:
        if (loadMode >= 1)
            return true;
        [[ fallthrough ]];
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

        std::shared_ptr<SpriteGlobal> psGlobal = nullptr;
        SpriteTypes sType = e->type();
        if (sType == SpriteTypes::GLOBAL)
        {
            // unpack stacking references
            auto p = std::reinterpret_pointer_cast<SpriteGlobal>(e);
            psGlobal = p;
            decltype(e) enext = nullptr;
            do
            {
                enext = gSprites[p->getIdx()];
                p->set(enext);

                if (enext == nullptr)
                {
                    LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous src definition invalid (Line: " << csvLineNumber << ")";
                    return false;
                }
                sType = enext->type();

            } while (sType == SpriteTypes::GLOBAL);
        }

        // check if type of previous src definition matches
        bool typeMatch = true;
        switch (type)
        {
        case DefType::IMAGE:         typeMatch = (sType == SpriteTypes::ANIMATED || sType == SpriteTypes::VIDEO); break;
        case DefType::NUMBER:        typeMatch = sType == SpriteTypes::NUMBER; break;
        case DefType::SLIDER:        typeMatch = sType == SpriteTypes::SLIDER; break;
        case DefType::BARGRAPH:      typeMatch = sType == SpriteTypes::BARGRAPH; break;
        case DefType::BUTTON:        typeMatch = (sType == SpriteTypes::BUTTON || sType == SpriteTypes::OPTION); break;
        case DefType::ONMOUSE:       typeMatch = sType == SpriteTypes::ONMOUSE; break;
        case DefType::TEXT:          typeMatch = (sType == SpriteTypes::TEXT || sType == SpriteTypes::IMAGE_TEXT); break;
        case DefType::JUDGELINE:     typeMatch = sType == SpriteTypes::ANIMATED; break;
        case DefType::GROOVEGAUGE:   typeMatch = sType == SpriteTypes::GAUGE; break;
        case DefType::NOWJUDGE_1P:   typeMatch = sType == SpriteTypes::ANIMATED; break;
        case DefType::NOWCOMBO_1P:   typeMatch = sType == SpriteTypes::NUMBER; break;
        case DefType::NOWJUDGE_2P:   typeMatch = sType == SpriteTypes::ANIMATED; break;
        case DefType::NOWCOMBO_2P:   typeMatch = sType == SpriteTypes::NUMBER; break;
        case DefType::BGA:           typeMatch = sType == SpriteTypes::STATIC; break;
        case DefType::MOUSECURSOR:   typeMatch = sType == SpriteTypes::MOUSE_CURSOR; break;
        case DefType::GAUGECHART_1P: typeMatch = sType == SpriteTypes::LINE; break;
        case DefType::GAUGECHART_2P: typeMatch = sType == SpriteTypes::LINE; break;
        case DefType::SCORECHART:    typeMatch = sType == SpriteTypes::LINE; break;
        }
        if (!typeMatch)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous src definition type mismatch (Line: " << csvLineNumber << ")";
            return false;
        }

        if (e->isKeyFrameEmpty())
        {
            if (lr2skin::flipSideFlag)
            {
                switch (type)
                {
                case DefType::NOWCOMBO_2P:
                case DefType::NOWJUDGE_2P:
                    switch (bufJudge1PSlot)
                    {
                    case 0: d.timer = (int)IndexTimer::_JUDGE_1P_0; break;
                    case 1: d.timer = (int)IndexTimer::_JUDGE_1P_1; break;
                    case 2: d.timer = (int)IndexTimer::_JUDGE_1P_2; break;
                    case 3: d.timer = (int)IndexTimer::_JUDGE_1P_3; break;
                    case 4: d.timer = (int)IndexTimer::_JUDGE_1P_4; break;
                    case 5: d.timer = (int)IndexTimer::_JUDGE_1P_5; break;
                    default: break;
                    }
                    break;

                case DefType::NOWCOMBO_1P:
                case DefType::NOWJUDGE_1P:
                    switch (bufJudge2PSlot)
                    {
                    case 0: d.timer = (int)IndexTimer::_JUDGE_2P_0; break;
                    case 1: d.timer = (int)IndexTimer::_JUDGE_2P_1; break;
                    case 2: d.timer = (int)IndexTimer::_JUDGE_2P_2; break;
                    case 3: d.timer = (int)IndexTimer::_JUDGE_2P_3; break;
                    case 4: d.timer = (int)IndexTimer::_JUDGE_2P_4; break;
                    case 5: d.timer = (int)IndexTimer::_JUDGE_2P_5; break;
                    default: break;
                    }
                    break;
                }
            }
            else
            {
                switch (type)
                {
                case DefType::NOWCOMBO_1P:
                case DefType::NOWJUDGE_1P:
                    switch (bufJudge1PSlot)
                    {
                    case 0: d.timer = (int)IndexTimer::_JUDGE_1P_0; break;
                    case 1: d.timer = (int)IndexTimer::_JUDGE_1P_1; break;
                    case 2: d.timer = (int)IndexTimer::_JUDGE_1P_2; break;
                    case 3: d.timer = (int)IndexTimer::_JUDGE_1P_3; break;
                    case 4: d.timer = (int)IndexTimer::_JUDGE_1P_4; break;
                    case 5: d.timer = (int)IndexTimer::_JUDGE_1P_5; break;
                    default: break;
                    }
                    break;

                case DefType::NOWCOMBO_2P:
                case DefType::NOWJUDGE_2P:
                    switch (bufJudge2PSlot)
                    {
                    case 0: d.timer = (int)IndexTimer::_JUDGE_2P_0; break;
                    case 1: d.timer = (int)IndexTimer::_JUDGE_2P_1; break;
                    case 2: d.timer = (int)IndexTimer::_JUDGE_2P_2; break;
                    case 3: d.timer = (int)IndexTimer::_JUDGE_2P_3; break;
                    case 4: d.timer = (int)IndexTimer::_JUDGE_2P_4; break;
                    case 5: d.timer = (int)IndexTimer::_JUDGE_2P_5; break;
                    default: break;
                    }
                    break;
                }
            }

            std::vector<dst_option> opEx;
            if (type == DefType::NUMBER)
            {
                auto p = std::reinterpret_pointer_cast<SpriteNumber>(e);
                switch (p->_numInd)
                {
                case IndexNumber::MUSIC_BEGINNER_LEVEL:  opEx.push_back(dst_option::SELECT_HAVE_BEGINNER_IN_SAME_FOLDER); break;
                case IndexNumber::MUSIC_NORMAL_LEVEL:    opEx.push_back(dst_option::SELECT_HAVE_NORMAL_IN_SAME_FOLDER);   break;
                case IndexNumber::MUSIC_HYPER_LEVEL:     opEx.push_back(dst_option::SELECT_HAVE_HYPER_IN_SAME_FOLDER);    break;
                case IndexNumber::MUSIC_ANOTHER_LEVEL:   opEx.push_back(dst_option::SELECT_HAVE_ANOTHER_IN_SAME_FOLDER);  break;
                case IndexNumber::MUSIC_INSANE_LEVEL:    opEx.push_back(dst_option::SELECT_HAVE_INSANE_IN_SAME_FOLDER);   break;
                }
            }
            drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), opEx });

            e->setLoopTime(d.loop);
            e->setTrigTimer((IndexTimer)d.timer);
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
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
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
    if (lr2skin::flipSideFlag)
    {
        if (0 <= d._null && d._null <= 9) d._null += 10;
        else if (10 <= d._null && d._null <= 19) d._null -= 10;
    }

    if (d._null >= 20)
    {
        return ParseRet::PARAM_INVALID;
    }

    NoteLaneIndex idx = NoteIdxToLane(info.mode, d._null);

    auto setDstNoteSprite = [&](NoteLaneCategory i, std::shared_ptr<SpriteLaneVertical> e)
    {
        if (e->isKeyFrameEmpty())
        {
            drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
            e->setLoopTime(d.loop);
            e->setTrigTimer((IndexTimer)d.timer);
        }
        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    };

    auto& [e1, e1a] = _laneSprites[channelToIdx(NoteLaneCategory::Note, idx)];
    if (e1) setDstNoteSprite(NoteLaneCategory::Note, e1);
    if (e1a) setDstNoteSprite(NoteLaneCategory::Note, e1a);

    auto& [e2, e2a] = _laneSprites[channelToIdx(NoteLaneCategory::Mine, idx)];
    if (e2) setDstNoteSprite(NoteLaneCategory::Mine, e2);
    if (e2a) setDstNoteSprite(NoteLaneCategory::Mine, e2a);

    auto& [e3, e3a] = _laneSprites[channelToIdx(NoteLaneCategory::LN, idx)];
    if (e3) setDstNoteSprite(NoteLaneCategory::LN, e3);
    if (e3a) setDstNoteSprite(NoteLaneCategory::LN, e3a);

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

    auto& [cat, idx] = p->getLane();
    if (cat != chart::NoteLaneCategory::EXTRA || (idx != chart::EXTRA_BARLINE_1P && idx != chart::EXTRA_BARLINE_2P))
    {
        LOG_WARNING << "[Skin] " << csvLineNumber << ": Previous SRC definition is not LINE " <<
            "(Line: " << csvLineNumber << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }

    p->pNote->clearKeyFrames();
    p->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

    drawQueue.push_back({ e, dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
    e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    e->setLoopTime(0);
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << raw << ": Set Lane sprite (Barline) Keyframe (time: " << d.time << ")";

    if (idx == chart::EXTRA_BARLINE_1P)
    {
        info.noteLaneHeight1P = d.y + d.h;
    }
    else if (idx == chart::EXTRA_BARLINE_2P)
    {
        info.noteLaneHeight2P = d.y + d.h;
    }

    return ParseRet::OK;
}


ParseRet SkinLR2::DST_BAR_BODY()
{
    bool bodyOn = parseKeyBuf == "#DST_BAR_BODY_ON";

    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
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
            e->setTrigTimer((IndexTimer)d.timer);

            if (!_barSpriteAdded[idx])
            {
                _barSprites[idx]->setSrcLine(csvLineNumber);
                _barSpriteAdded[idx] = true;
                drawQueue.push_back({ _barSprites[idx], dst_option(d.op[0]), dst_option(d.op[1]), dst_option(d.op[2]), dst_option(d.op[3]), {} });
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    }

    _barSprites[idx]->pushPartsOrder(bodyOn ? BarPartsType::BODY_ON : BarPartsType::BODY_OFF);

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_FLASH()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::FLASH);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LEVEL()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
    BarLevelType type = BarLevelType(d._null);
    if (d._null >= (int)BarLevelType::LEVEL_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry level type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LEVEL);
    }


    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL_MYLAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
    auto type = BarLampType(d._null);
    if (d._null >= (int)BarLampType::LAMP_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry mylamp type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::MYLAMP);
    }

    return ParseRet::OK;
}
ParseRet SkinLR2::DST_BAR_RIVAL_RIVALLAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
    auto type = BarLampType(d._null);
    if (d._null >= (int)BarLampType::LAMP_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry rivallamp type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RIVALLAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LAMP()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
    auto type = BarLampType(d._null);
    if (d._null >= (int)BarLampType::LAMP_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry lamp type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_TITLE()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;

    auto type = BarTitleType(d._null);
    if (d._null >= (int)BarTitleType::TITLE_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry title type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::TITLE);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RANK()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;

    auto type = BarRankType(d._null);
    if (d._null >= (int)BarRankType::RANK_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry rank type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RANK);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL()
{
    // load raw into data struct
    lr2skin::dst d(parseParamBuf);

    // timers are ignored for bars
    d.timer = 0;
    
    auto type = BarRivalType(d._null);
    if (d._null >= (int)BarRivalType::RIVAL_TYPE_COUNT)
    {
        LOG_WARNING << "[SkinLR2] BarEntry rival type (" << int(type) << ") Invalid!"
            << " (Line " << csvLineNumber << ")";
        return ParseRet::PARAM_INVALID;
    }

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
            e->setTrigTimer((IndexTimer)d.timer);
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            lr2skin::convertBlend(d.blend), !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

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
    if (parseKeyBuf.empty()) return 0;
    for (size_t idx = 0; idx < parseParamBuf.size(); ++idx)
    {
        parseParamBuf[idx] = raw[idx + 1];

#ifdef WIN32
        static const auto localeUTF8 = std::locale(".65001");
#else
        static const auto localeUTF8 = std::locale("en_US.UTF-8");
#endif
        boost::trim(parseParamBuf[idx], localeUTF8);
    }

    if (strEqual(parseKeyBuf, "#INFORMATION", true))
    {
        while (parseParamBuf.size() < 4) parseParamBuf.push_back("");

        int type = toInt(parseParamBuf[0]);
        auto& title = StringContent(parseParamBuf[1]);
        auto& maker = StringContent(parseParamBuf[2]);
        StringContent thumbnail(parseParamBuf[3]);

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
        case 12:	info.mode = eMode::PLAY7_2;	break;
        case 13:	info.mode = eMode::PLAY5_2;	break;
        case 14:	info.mode = eMode::PLAY9_2;	break;
        case 15:	info.mode = eMode::COURSE_RESULT;	break;

        case 17:	info.mode = eMode::TITLE;	break;
        }
        info.name = title;
        info.maker = maker;

        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Loaded metadata: " << title << " | " << maker;

        return 1;
    }

    else if (strEqual(parseKeyBuf, "#CUSTOMOPTION", true))
    {
        while (parseParamBuf.size() < 2) parseParamBuf.push_back("");

        auto& title(parseParamBuf[0]);
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
        while (parseParamBuf.size() < 3) parseParamBuf.push_back("");

        auto& title(parseParamBuf[0]);
        auto& path(parseParamBuf[1]);
        Path pathf = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), path));
        auto& def(parseParamBuf[2]);

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

        LOG_DEBUG << "[Skin] " << csvLineNumber << ": Loaded Custom file " << title << ": " << pathf.u8string();

        Customize c;
        c.type = Customize::_Type::FILE;
        c.title = title;
        c.dst_op = 0;
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
        customize.push_back(c);

        std::srand(std::time(NULL));
        customizeRandom[customize.size() - 1] = ls.empty() ? 0 : (std::rand() % ls.size());

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
    {
        parseParamBuf[idx] = raw[idx + 1];

#ifdef WIN32
        static const auto localeUTF8 = std::locale(".65001");
#else
        static const auto localeUTF8 = std::locale("en_US.UTF-8");
#endif
        boost::trim(parseParamBuf[idx], localeUTF8);
    }

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

void SkinLR2::IF(const Tokens &t, std::istream& lr2skin, eFileEncoding enc, bool ifUnsatisfied, bool skipOnly)
{
    if (skipOnly)
    {
        // only look for #ENDIF, skip the whole sub #IF block
        while (!lr2skin.eof())
        {
            std::string raw;
            std::getline(lr2skin, raw);
            ++csvLineNumber;

            std::string rawUTF8 = to_utf8(raw, enc);

            auto tokens = csvLineTokenize(rawUTF8);
            if (tokens.empty()) continue;

            if (strEqual(*tokens.begin(), "#IF", true))
            {
                // nesting #IF
                IF(tokens, lr2skin, enc, false, true);
            }
            else if (strEqual(*tokens.begin(), "#ENDIF", true))
            {
                // end #IF process
                return;
            }
        }
    }

    bool ifStmtTrue = false;
    if (ifUnsatisfied && strEqual(t[0], "#ELSE", true))
    {
        ifStmtTrue = true;
    }
    else
    {
        if (t.size() <= 1)
        {
            LOG_WARNING << "[Skin] " << csvLineNumber << ": No IF parameters " << " (Line " << csvLineNumber << ")";
        }

        // get dst indexes
        ifStmtTrue = true;
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

            std::string rawUTF8 = to_utf8(raw, enc);

            auto tokens = csvLineTokenize(rawUTF8);
            if (tokens.empty()) continue;

            if (!ifBlockEnded)
            {
                // parse current branch
                if (strEqual(*tokens.begin(), "#ELSEIF", true) || strEqual(*tokens.begin(), "#ELSE", true))
                {
                    IF(tokens, lr2skin, enc, false, true);
                    break;
                }
                else if (strEqual(*tokens.begin(), "#IF", true))
                {
                    // nesting #IF
                    IF(tokens, lr2skin, enc, false, false);
                }
                else if (strEqual(*tokens.begin(), "#ENDIF", true))
                {
                    // end #IF process
                    return;
                }
                else
                {
                    parseBody(tokens);
                }
            }
            else
            {
                if (strEqual(*tokens.begin(), "#IF", true))
                {
                    // nesting #IF
                    IF(tokens, lr2skin, enc, false, true);
                }
                else if (strEqual(*tokens.begin(), "#ELSEIF", true) || strEqual(*tokens.begin(), "#ELSE", true))
                {
                    IF(tokens, lr2skin, enc, false, true);
                    break;
                }
                else if (strEqual(*tokens.begin(), "#ENDIF", true))
                {
                    // end #IF process
                    return;
                }
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

            if (strEqual(*tokens.begin(), "#IF", true))
            {
                // nesting #IF
                IF(tokens, lr2skin, enc, false, true);
            }
            else if (strEqual(*tokens.begin(), "#ELSE", true))
            {
                IF(tokens, lr2skin, enc, true, false);
                return;
            }
            else if (strEqual(*tokens.begin(), "#ELSEIF", true))
            {
                IF(tokens, lr2skin, enc, true, false);
                return;
            }
            else if (strEqual(*tokens.begin(), "#ENDIF", true))
            {
                return;
            }
        }
    }
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

SkinLR2::SkinLR2(Path p, int loadMode): loadMode(loadMode)
{
    _type = eSkinType::LR2;

    for (size_t i = 0; i < BAR_ENTRY_SPRITE_COUNT; ++i)
    {
        _barSprites[i] = std::make_shared<SpriteBarEntry>(i);
        _sprites.push_back(_barSprites[i]);
    }
    _laneSprites.resize(chart::LANE_COUNT);

    // re-reference previously loaded #IMAGE
    _textureNameMap.insert(LR2SkinImageCache.begin(), LR2SkinImageCache.end());

    switch (info.mode)
    {
    case eMode::PLAY5:
    case eMode::PLAY5_2:
    case eMode::PLAY7:
    case eMode::PLAY7_2:
    case eMode::PLAY9:
    case eMode::PLAY10:
    case eMode::PLAY14:
        flipResult = false;
        break;
    }

    updateDstOpt();
    if (loadCSV(p))
    {
        postLoad();

        LOG_DEBUG << "[Skin] File: " << p.u8string() << "(Line " << csvLineNumber << "): Body loading finished";
        _loaded = true;

        startSpriteVideoPlayback();
    }
}

SkinLR2::~SkinLR2()
{
    stopSpriteVideoPlayback();
}

bool SkinLR2::loadCSV(Path p)
{
    if (filePath.empty())
        filePath = p;

    auto srcLineNumberParent = csvLineNumber;
    csvLineNumber = 0;

    p = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), p));

    std::ifstream ifsFile(p, std::ios::binary);
    if (!ifsFile.is_open())
    {
        LOG_ERROR << "[Skin] File Not Found: " << std::filesystem::absolute(p).u8string();
        csvLineNumber = srcLineNumberParent;
        return false;
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
        std::string rawUTF8 = to_utf8(raw, encoding);

        auto tokens = csvLineTokenize(rawUTF8);
        if (tokens.empty()) continue;

        if (parseHeader(tokens) == -1)
        {
            // #ENDOFHEADER
            haveEndOfHeader = true;
            break;
        }
    }
    LOG_DEBUG << "[Skin] File: " << p.u8string() << "(Line " << csvLineNumber << "): Header loading finished";

    if (loadMode < 2)
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

            std::string rawUTF8 = to_utf8(raw, encoding);

            auto tokens = csvLineTokenize(rawUTF8);
            if (tokens.empty()) continue;

            if (strEqual(*tokens.begin(), "#IF", true))
            {
                IF(tokens, csvFile, encoding);
            }
            else if (strEqual(*tokens.begin(), "#ELSE", true))
            {
                LOG_WARNING << "[Skin] Unexcepted #ELSE found without precedent #IF " << "(Line " << csvLineNumber << ")";
            }
            else if (strEqual(*tokens.begin(), "#ELSEIF", true))
            {
                LOG_WARNING << "[Skin] Unexcepted #ELSEIF found without precedent #IF " << "(Line " << csvLineNumber << ")";
            }
            else if (strEqual(*tokens.begin(), "#ENDIF", true))
            {
                LOG_WARNING << "[Skin] Unexcepted #ENDIF found without precedent #IF " << "(Line " << csvLineNumber << ")";
            }
            else
            {
                parseBody(tokens);
            }
        }
    }

    csvLineNumber = srcLineNumberParent;
    return true;
}

void SkinLR2::postLoad()
{
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
            pS->setHeight(500);
        }
    }

    auto setLaneHeight = [&](size_t begin, size_t end, int height)
    {
        using namespace chart;
        for (size_t i = begin; i <= end; ++i)
        {
            NoteLaneIndex lane = NoteIdxToLane(info.mode, i);
            if (lane == _) continue;
            size_t idx;

            idx = channelToIdx(NoteLaneCategory::Note, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHeight(height);
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHeight(height);
            }
            idx = channelToIdx(NoteLaneCategory::Mine, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHeight(height);
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHeight(height);
            }
            idx = channelToIdx(NoteLaneCategory::LN, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHeight(height);
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHeight(height);
            }
        }
    };
    if (info.noteLaneHeight1P != 0)
    {
        using namespace chart;
        setLaneHeight(0, 9, info.noteLaneHeight1P);

        constexpr size_t idx = channelToIdx(NoteLaneCategory::EXTRA, NoteLaneExtra::EXTRA_BARLINE_1P);
        if (idx != LANE_INVALID)
        {
            if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHeight(info.noteLaneHeight1P);
            if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHeight(info.noteLaneHeight1P);
        }
    }
    if (info.noteLaneHeight2P != 0)
    {
        using namespace chart;
        setLaneHeight(10, 19, info.noteLaneHeight2P);

        constexpr size_t idx = channelToIdx(NoteLaneCategory::EXTRA, NoteLaneExtra::EXTRA_BARLINE_2P);
        if (idx != LANE_INVALID)
        {
            if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHeight(info.noteLaneHeight2P);
            if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHeight(info.noteLaneHeight2P);
        }
    }

    if (!isSupportLift)
    {
        using namespace chart;
        size_t idx;
        for (size_t i = 0; i < 20; ++i)
        {
            NoteLaneIndex lane = NoteIdxToLane(info.mode, i);
            if (lane == _) continue;

            idx = channelToIdx(NoteLaneCategory::Note, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHIDDENCompatible();
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHIDDENCompatible();
            }
            idx = channelToIdx(NoteLaneCategory::Mine, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHIDDENCompatible();
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHIDDENCompatible();
            }
            idx = channelToIdx(NoteLaneCategory::LN, lane);
            if (idx != LANE_INVALID)
            {
                if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHIDDENCompatible();
                if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHIDDENCompatible();
            }
        }
        idx = channelToIdx(NoteLaneCategory::EXTRA, NoteLaneExtra::EXTRA_BARLINE_1P);
        if (idx != LANE_INVALID)
        {
            if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHIDDENCompatible();
            if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHIDDENCompatible();
        }
        idx = channelToIdx(NoteLaneCategory::EXTRA, NoteLaneExtra::EXTRA_BARLINE_2P);
        if (idx != LANE_INVALID)
        {
            if (_laneSprites[idx].first != nullptr) _laneSprites[idx].first->setHIDDENCompatible();
            if (_laneSprites[idx].second != nullptr) _laneSprites[idx].second->setHIDDENCompatible();
        }
    }

    if (gSprites[GLOBAL_SPRITE_IDX_1PJUDGELINE] && !gSprites[GLOBAL_SPRITE_IDX_1PJUDGELINE]->_keyFrames.empty())
    {
        Rect d = gSprites[GLOBAL_SPRITE_IDX_1PJUDGELINE]->_keyFrames.back().param.rect;
        judgeLineRect1P = d;
        if (d.w < 0) { judgeLineRect1P.x += d.w; judgeLineRect1P.w = -d.w; }
        if (d.h < 0) { judgeLineRect1P.y += d.h; judgeLineRect1P.h = -d.h; }
    }
    if (gSprites[GLOBAL_SPRITE_IDX_2PJUDGELINE] && !gSprites[GLOBAL_SPRITE_IDX_2PJUDGELINE]->_keyFrames.empty())
    {
        Rect d = gSprites[GLOBAL_SPRITE_IDX_2PJUDGELINE]->_keyFrames.back().param.rect;
        judgeLineRect2P = d;
        if (d.w < 0) { judgeLineRect2P.x += d.w; judgeLineRect2P.w = -d.w; }
        if (d.h < 0) { judgeLineRect2P.y += d.h; judgeLineRect2P.h = -d.h; }
    }

    // LIFT:
    // the following conditions are based on black box testing on LR2
    // 
    // SRC_IMAGE (background):
    //  JUDGELINE.x - 5  <= x <= JUDGELINE.x + 5
    //  JUDGELINE.w - 10 <= w <= JUDGELINE.w + 10
    //  y <= JUDGELINE.y
    //
    // SRC_IMAGE with bomb timers:
    //  did not found any obvious conditions
    // 
    // SRC_IMAGE with key input timers (laser):
    //  h >= 100
    //
    // the following conditions are added by Lunatic Vibes
    // 
    // SRC_NUMBER0:
    // num=108/128 (target score): timer=40/46, y <= JUDGELINE.y
    // num=210/211 (F/S): timer=40/46, y <= JUDGELINE.y
    // num=310-315 (3col)
    for (auto& e : drawQueue)
    {
        auto& s = e.ps;
        if (s->isKeyFrameEmpty()) continue;
        if (s->type() == SpriteTypes::ANIMATED)
        {
            const Rect& rcFirst = s->_keyFrames.front().param.rect;
            const Rect& rcLast = s->_keyFrames.back().param.rect;
            int timer = (int)s->_triggerTimer;
            if (timer >= 100 && timer <= 109 || timer >= 120 && timer <= 129 ||
                timer == (int)IndexTimer::S1L_DOWN || timer == (int)IndexTimer::S1L_UP || timer == (int)IndexTimer::S1R_DOWN || timer == (int)IndexTimer::S1R_UP)
            {
                // 1P laser
                if (rcFirst.h <= -100 || rcFirst.h >= 100 || rcLast.h <= -100 || rcLast.h >= 100)
                    spritesMoveWithLift1P.push_back(s);
            }
            else if (timer >= 110 && timer <= 119 || timer >= 130 && timer <= 139 ||
                timer == (int)IndexTimer::S2L_DOWN || timer == (int)IndexTimer::S2L_UP || timer == (int)IndexTimer::S2R_DOWN || timer == (int)IndexTimer::S2R_UP)
            {
                // 2P laser
                if (rcFirst.h <= -100 || rcFirst.h >= 100 || rcLast.h <= -100 || rcLast.h >= 100)
                    spritesMoveWithLift2P.push_back(s);
            }
            else if (timer >= 50 && timer <= 59 || timer >= 70 && timer <= 79)
            {
                // 1P bomb
                spritesMoveWithLift1P.push_back(s);
            }
            else if (timer >= 60 && timer <= 69 || timer >= 80 && timer <= 89)
            {
                // 2P bomb
                spritesMoveWithLift2P.push_back(s);
            }
            else if (judgeLineRect1P.x - 5 <= rcLast.x && rcLast.x <= judgeLineRect1P.x + 5 &&
                judgeLineRect1P.w - 10 <= rcLast.w && rcLast.w <= judgeLineRect1P.w + 10 &&
                rcLast.y <= judgeLineRect1P.y)
            {
                // 1P background
                spritesMoveWithLift1P.push_back(s);
            }
            else if (judgeLineRect2P.x - 5 <= rcLast.x && rcLast.x <= judgeLineRect2P.x + 5 &&
                judgeLineRect2P.w - 10 <= rcLast.w && rcLast.w <= judgeLineRect2P.w + 10 &&
                rcLast.y <= judgeLineRect2P.y)
            {
                // 2P background
                spritesMoveWithLift2P.push_back(s);
            }
        }
        else if (s->type() == SpriteTypes::NUMBER)
        {
            const Rect& rcFirst = s->_keyFrames.front().param.rect;
            const Rect& rcLast = s->_keyFrames.back().param.rect;
            int num = (int)std::dynamic_pointer_cast<SpriteNumber>(s)->_numInd;
            int timer = (int)s->_triggerTimer;
            if (timer == 40 || timer == 46)
            {
                if (rcFirst.y <= judgeLineRect1P.y || rcLast.y <= judgeLineRect1P.y)
                {
                    if (num == 108 || num == 210)
                    {
                        // 1P target score, F/S
                        spritesMoveWithLift1P.push_back(s);
                    }
                }
                else if (rcFirst.y <= judgeLineRect2P.y || rcLast.y <= judgeLineRect2P.y)
                {
                    if (num == 128 || num == 211)
                    {
                        // 1P target score, F/S
                        spritesMoveWithLift2P.push_back(s);
                    }
                }
            }
            else if (num >= 310 && num <= 315)
            {
                // 3col judge
                spritesMoveWithLift1P.push_back(s);
            }
        }
    }

    // customize: replace timer 0 with internal
    if (info.mode == eMode::THEME_SELECT)
    {
        for (auto& s : _sprites)
        {
            if (s->_triggerTimer == IndexTimer::SCENE_START)
                s->_triggerTimer = IndexTimer::_SCENE_CUSTOMIZE_START;
            if (s->_triggerTimer == IndexTimer::FADEOUT_BEGIN)
                s->_triggerTimer = IndexTimer::_SCENE_CUSTOMIZE_FADEOUT;

            auto p = std::dynamic_pointer_cast<SpriteAnimated>(s);
            if (p)
            {
                if (p->_resetAnimTimer == IndexTimer::SCENE_START)
                    p->_resetAnimTimer = IndexTimer::_SCENE_CUSTOMIZE_START;
                if (p->_triggerTimer == IndexTimer::FADEOUT_BEGIN)
                    p->_triggerTimer = IndexTimer::_SCENE_CUSTOMIZE_FADEOUT;
            }
        }
    }
}

void SkinLR2::findAndExtractDXA(const Path& path)
{
    if (!fs::is_regular_file(path))
    {
        std::string archiveName;
        Path lr2skinFolder = fs::absolute(filePath).parent_path();
        auto lr2skinFolderStr = lr2skinFolder.native();
        Path folder = path.parent_path();
        Path::string_type folderStr;
        do
        {
            archiveName = folder.stem().u8string() + ".dxa";
            folder = folder.parent_path();
            folderStr = fs::absolute(folder).native();

            // find dxa file
            Path dxa = folder / PathFromUTF8(archiveName);

            // extract dxa
            if (std::filesystem::is_regular_file(dxa))
            {
                LOG_DEBUG << "[Skin] Extract dxa file: " << fs::absolute(dxa).u8string();
                extractDxaToFile(dxa);
                break;
            }
        } while (folderStr.length() >= lr2skinFolderStr.length() && folderStr.substr(0, lr2skinFolderStr.length()) == lr2skinFolderStr);
    }
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
        int ttAngle1P = State::get(IndexNumber::_ANGLE_TT_1P);
        int ttAngle2P = State::get(IndexNumber::_ANGLE_TT_2P);

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
    // 0-5:   NOWJUDGE 1P
    // 6-11:  NOWCOMBO 1P
    // 12-17: NOWJUDGE 2P
    // 18-23: NOWCOMBO 2P
    for (size_t i = 0; i < 6; ++i)
    {
        // 1P judge
        if (gSprites[i] && gSprites[i + 6])
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 6]);
            if (judge->isDraw() && !judge->isHidden())
            {
                combo->setHide(false);

                Rect base = judge->_current.rect;
                double shiftUnit = 0.5 * combo->_current.rect.w;

                int judgeShiftWidth = noshiftJudge1P[i] ? 0 : int(std::floor(shiftUnit * combo->_numDigits));
                judge->_current.rect.x -= judgeShiftWidth;

                for (auto& d : combo->_rects)
                {
                    int comboShiftUnitCount = noshiftJudge1P[i] ? 0 : -1;

                    switch (alignNowCombo1P[i])
                    {
                    case 0:
                        comboShiftUnitCount += (combo->_maxDigits - combo->_numDigits) * 2 - combo->_numDigits + 1;
                        break;
                    case 1:
                        comboShiftUnitCount -= combo->_numDigits - 1;
                        break;
                    case 2:
                        comboShiftUnitCount += (-combo->_maxDigits - 1) + (combo->_maxDigits - combo->_numDigits + 1) * 2;
                        break;
                    }
                    d.x += base.x + int(std::floor(shiftUnit * comboShiftUnitCount));
                    d.y += base.y;
                }
            }
            else
            {
                combo->setHide(true);
            }
        }
        // 2P judge
        if (gSprites[i + 12] && gSprites[i + 18])
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i + 12]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 18]);
            if (judge->isDraw() && !judge->isHidden())
            {
                combo->setHide(false);

                Rect base = judge->_current.rect;
                double shiftUnit = 0.5 * combo->_current.rect.w;

                int judgeShiftWidth = noshiftJudge2P[i] ? 0 : int(std::floor(shiftUnit * combo->_numDigits));
                judge->_current.rect.x -= judgeShiftWidth;

                for (auto& d : combo->_rects)
                {
                    int comboShiftUnitCount = noshiftJudge2P[i] ? 0 : -1;

                    switch (alignNowCombo2P[i])
                    {
                    case 0:
                        comboShiftUnitCount += (combo->_maxDigits - combo->_numDigits) * 2 - combo->_numDigits + 1;
                        break;
                    case 1:
                        comboShiftUnitCount -= combo->_numDigits - 1;
                        break;
                    case 2:
                        comboShiftUnitCount += (-combo->_maxDigits - 1) + (combo->_maxDigits - combo->_numDigits + 1) * 2;
                        break;
                    }
                    d.x += base.x + int(std::floor(shiftUnit * comboShiftUnitCount));
                    d.y += base.y;
                }
            }
            else
            {
                combo->setHide(true);
            }
        }
    }

    // LIFT: move judgeline, nowjudge, nowcombo
    // 0-5:   NOWJUDGE 1P
    // 6-11:  NOWCOMBO 1P
    // 12-17: NOWJUDGE 2P
    // 18-23: NOWCOMBO 2P
    // 26: GLOBAL_SPRITE_IDX_1PJUDGELINE
    // 27: GLOBAL_SPRITE_IDX_2PJUDGELINE
    int lift1P = 0, lift2P = 0;
    if (gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect == eModLaneEffect::LIFT ||
        gPlayContext.mods[PLAYER_SLOT_PLAYER].laneEffect == eModLaneEffect::LIFTSUD)
    {
        lift1P = (State::get(IndexNumber::LANECOVER_BOTTOM_1P) / 1000.0) * info.noteLaneHeight1P;
        if (gPlayContext.mode == eMode::PLAY10 || gPlayContext.mode == eMode::PLAY14)
            lift2P = (State::get(IndexNumber::LANECOVER_BOTTOM_1P) / 1000.0) * info.noteLaneHeight2P;
    }
    if (gPlayContext.isBattle && 
        (gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect == eModLaneEffect::LIFT ||
         gPlayContext.mods[PLAYER_SLOT_TARGET].laneEffect == eModLaneEffect::LIFTSUD))
    {
        lift2P = (State::get(IndexNumber::LANECOVER_BOTTOM_2P) / 1000.0) * info.noteLaneHeight2P;
    }
    gUpdateContext.liftHeight1P = lift1P;
    gUpdateContext.liftHeight2P = lift2P;
    if (lift1P > 0)
    {
        for (size_t i = 0; i < 6; ++i)
        {
            if (gSprites[i])
            {
                std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i]);
                if (judge->isDraw() && !judge->isHidden())
                {
                    judge->moveAfterUpdate(0, -lift1P);
                }
            }
            if (gSprites[i + 6])
            {
                std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 6]);
                if (combo->isDraw() && !combo->isHidden())
                {
                    combo->moveAfterUpdate(0, -lift1P);
                }
            }
        }
        std::shared_ptr<SpriteAnimated> judgeLine = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[GLOBAL_SPRITE_IDX_1PJUDGELINE]);
        if (judgeLine && judgeLine->isDraw() && !judgeLine->isHidden())
        {
            judgeLine->moveAfterUpdate(0, -lift1P);
        }
    }
    if (lift2P > 0)
    {
        for (size_t i = 0; i < 6; ++i)
        {
            if (gSprites[i + 12])
            {
                std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i + 12]);
                if (judge->isDraw() && !judge->isHidden())
                {
                    judge->moveAfterUpdate(0, -lift2P);
                }
            }
            if (gSprites[i + 18])
            {
                std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 18]);
                if (combo->isDraw() && !combo->isHidden())
                {
                    combo->moveAfterUpdate(0, -lift2P);
                }
            }
        }
        std::shared_ptr<SpriteAnimated> judgeLine = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[GLOBAL_SPRITE_IDX_2PJUDGELINE]);
        if (judgeLine && judgeLine->isDraw() && !judgeLine->isHidden())
        {
            judgeLine->moveAfterUpdate(0, -lift2P);
        }
    }

    // LIFT: sprites
    if (lift1P > 0)
    {
        for (auto& s : spritesMoveWithLift1P)
            s->moveAfterUpdate(0, -lift1P);
    }
    if (lift2P > 0)
    {
        for (auto& s : spritesMoveWithLift2P)
            s->moveAfterUpdate(0, -lift2P);
    }

    // update songlist bar
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex, std::try_to_lock); // read lock
    if (u.owns_lock())
    {
        for (auto& s : _barSprites) s->update(t);

        // update songlist position
        if (hasBarAnimOrigin && gSelectContext.scrollDirection != 0 && !gSelectContext.entries.empty())
        {
            for (size_t i = 1; i + 1 < _barSprites.size(); ++i)
            {
                if (!_barSpriteAdded[i]) continue;

                double posNow = State::get(IndexSlider::SELECT_LIST) * gSelectContext.entries.size();

                double decimal = posNow - (int)posNow;
                if (decimal <= 0.5 && _barSprites[i - 1]->isDraw())
                {
                    double factor = decimal;
                    auto& rectStored = _barAnimOrigin[i - 1];
                    auto& rectSprite = _barSprites[i]->_current.rect;
                    Rect dr{
                        static_cast<int>(std::round((rectStored.x - rectSprite.x) * factor)),
                        static_cast<int>(std::round((rectStored.y - rectSprite.y) * factor)),
                        0, 0
                    };
                    _barSprites[i]->setRectOffset(dr);
                }
                else if (_barSprites[i + 1]->isDraw())
                {
                    double factor = -decimal + 1.0;
                    auto& rectStored = _barAnimOrigin[i + 1];
                    auto& rectSprite = _barSprites[i]->_current.rect;
                    Rect dr{
                        static_cast<int>(std::round((rectStored.x - rectSprite.x) * factor)),
                        static_cast<int>(std::round((rectStored.y - rectSprite.y) * factor)),
                        0, 0
                    };
                    _barSprites[i]->setRectOffset(dr);
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

void SkinLR2::start_bar_animation()
{
    for (size_t i = 0; i < BAR_ENTRY_SPRITE_COUNT; ++i)
    {
        _barAnimOrigin[i] = _barSprites[i]->_current.rect;
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
