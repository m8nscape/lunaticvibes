#include "skin_lr2.h"
#include <plog/Log.h>
#include "utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <variant>
#include <execution>
#include "game/data/option.h"
#include "game/data/switch.h"

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

std::vector<std::variant<std::monostate, eSwitch, eOption>> buttonAdapter{
	std::monostate(),

	// 1~9
	eSwitch::SELECT_PANEL1,
	eSwitch::SELECT_PANEL2,
	eSwitch::SELECT_PANEL3,
	eSwitch::SELECT_PANEL4,
	eSwitch::SELECT_PANEL5,
	eSwitch::SELECT_PANEL6,
	eSwitch::SELECT_PANEL7,
	eSwitch::SELECT_PANEL8,
	eSwitch::SELECT_PANEL9,
	
	// 10~12
	eOption::SELECT_FILTER_DIFF,
	eOption::SELECT_FILTER_KEYS,
	eOption::SELECT_SORT,

	// 13~19
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,

	// 20~28
	eOption::SOUND_FX0,
	eOption::SOUND_FX1,
	eOption::SOUND_FX2,
	eSwitch::SOUND_FX0,
	eSwitch::SOUND_FX1,
	eSwitch::SOUND_FX2,
	eOption::SOUND_TARGET_FX0,
	eOption::SOUND_TARGET_FX1,
	eOption::SOUND_TARGET_FX2,

	//29~30
	eSwitch::_FALSE,	// EQ off/on
	eSwitch::_FALSE,	// EQ Preset

	//31~33
	eSwitch::SOUND_VOLUME,		// volume control
	eSwitch::SOUND_PITCH,
	eOption::SOUND_PITCH_TYPE,

	//34~39
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,

	//40~45
	eOption::PLAY_GAUGE_TYPE_1P,
	eOption::PLAY_GAUGE_TYPE_2P,
	eOption::PLAY_RANDOM_TYPE_1P,
	eOption::PLAY_RANDOM_TYPE_2P,
	eSwitch::PLAY_OPTION_AUTOSCR_1P,
	eSwitch::PLAY_OPTION_AUTOSCR_2P,
	
	//46~49
	eSwitch::_FALSE,	// shutter?
	eSwitch::_FALSE,
	eSwitch::_FALSE,	// reserved
	eSwitch::_FALSE,	// reserved

	//50~51
	eOption::PLAY_LANE_EFFECT_TYPE_1P,
	eOption::PLAY_LANE_EFFECT_TYPE_2P,

	eSwitch::_FALSE,	// reserved
	eSwitch::_FALSE,	// reserved

	//54
	eSwitch::PLAY_OPTION_DP_FLIP,
	eOption::PLAY_HSFIX_TYPE_1P,
	eOption::PLAY_BATTLE_TYPE,
	eSwitch::_FALSE,	// HS-1P
	eSwitch::_FALSE,	// HS-2P

	// 59~69
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,

	// 70
	eSwitch::SYSTEM_SCOREGRAPH,
	eOption::PLAY_GHOST_TYPE_1P,
	eSwitch::_TRUE,	// bga off/on/autoplay only, special
	eSwitch::_TRUE, // bga normal/extend, special
	eSwitch::_FALSE,// JUDGE TIMING
	eSwitch::_FALSE,// AUTO ADJUST, not supported
	eSwitch::_FALSE, // default target rate
	eSwitch::_FALSE, // target

	eSwitch::_FALSE,
	eSwitch::_FALSE,

		// 80
	eSwitch::_TRUE, // screen mode full/window, special
	eSwitch::_FALSE, // color mode, 32bit fixed
	eSwitch::_TRUE, // vsync, special
	eSwitch::_FALSE,//save replay, not supported
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,

		//90
	eSwitch::_FALSE,//off/favorite/ignore, not supported
	eSwitch::_FALSE,	// select all
	eSwitch::_FALSE,	// select beginner
	eSwitch::_FALSE,	// select normal
	eSwitch::_FALSE,	// select hyper
	eSwitch::_FALSE,	// select another
	eSwitch::_FALSE,	// select insane

	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,
	eSwitch::_FALSE,

		// 101


};

namespace lr2skin
{

    struct s_basic
    {
        int _null;
        int gr;
        int x, y, w, h;
        int div_x, div_y;
        int cycle;
        int timer;
    };

    struct s_image : s_basic
    {
        int op1, op2, op3;
    };

    struct s_number : s_basic
    {
        int num;
        int align;
        int keta;
    };

    struct s_slider : s_basic
    {
        int muki;
		int range;
        int type;
		int disable;
    };

    struct s_bargraph : s_basic
    {
        int type;
        int muki;
    };

    struct s_button : s_basic
    {
		int type;
		int click;
		int panel;
    };

    struct s_onmouse : s_basic
    {
        int panel;
        int x2, y2, w2, h2;
    };

    struct s_text
    {
        int _null;
        int font;
        int st;
        int align;
        int edit;
        int panel;
    };

    struct s_bga
    {
        int _null[10];
        int nobase;
        int nolayer;
        int nopoor;
    };

    typedef s_basic s_judgeline;
    typedef s_basic s_barline;
    typedef s_basic s_note;

    struct s_nowjudge : s_basic
    {
        int noshift;
    };

    struct s_nowcombo : s_basic
    {
        int _null2;
        int align;
        int keta;
    };

    struct s_groovegauge : s_basic
    {
        int add_x;
        int add_y;
    };

    typedef s_basic s_barbody;
    typedef s_basic s_barflash;
    typedef s_nowcombo s_barlevel;
    typedef s_basic s_barlamp;
    struct s_bartitle
    {
        int _null;
        int font;
        int st;
        int align;
    };
    typedef s_basic s_barrank;
    typedef s_basic s_barrival;
    struct s_readme
    {
        int _null;
        int font;
        int _null2[2];
        int kankaku;
    };
    typedef s_basic s_mousecursor;

    struct s_gaugechart : s_basic
    {
        int field_w, field_h;
        int start;
        int end;
    };
    typedef s_gaugechart s_scorechart;

    struct dst
    {
        int _null;
        int time;
        int x, y, w, h;
        int acc;
        int a, r, g, b;
        int blend;
        int filter;
        int angle;
        int center;
        int loop;
        int timer;
        dst_option op[4];
    };

}

int SkinLR2::setExtendedProperty(std::string& key, void* value)
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

#pragma region LR2 csv parsing

Tokens SkinLR2::csvNextLineTokenize(std::istream& file)
{
    ++line;
    StringContent linecsv;
    std::getline(file, linecsv);
    if (linecsv.empty()) return {};

    // remove leading spaces
    if (linecsv[0] == ' ') linecsv = linecsv.substr(linecsv.find_first_not_of(' '));
    if (linecsv.empty()) return {};

    // skip comments
    if (linecsv.substr(0, 2) == "//") return {};

    // remove trailing \r
    while (linecsv.length() > 0 && linecsv[linecsv.length() - 1] == '\r')
        linecsv.pop_back();
    if (linecsv.empty()) return {};

    const std::regex re{ R"(((?:[^\\,]|\\.)*?)(?:,|$))" };
    Tokens result = { std::sregex_token_iterator(linecsv.begin(), linecsv.end(), re, 1), std::sregex_token_iterator() };

    // set first token uppercase
    for (auto& c : result[0])
        if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';

    // remove trailing empty tokens
    size_t lastToken;
    for (lastToken = result.size() - 1; lastToken >= 0 && result[lastToken].empty(); --lastToken);
    result.resize(lastToken + 1);

    return result;
}

int convertLine(const Tokens& t, int* pData, size_t start = 0, size_t end = sizeof(lr2skin::s_basic) / sizeof(int))
{
    for (size_t i = start; i < end && i+1 < t.size(); ++i)
    {
        pData[i] = stoine(t[i + 1]);
    }
    return end;
}

void refineRect(lr2skin::s_basic& d, const Rect rect, unsigned line)
{
	if (d.w == -1)
		d.w = rect.w;
	if (d.h == -1)
		d.h = rect.h;

    if (d.div_x == 0)
    {
        LOG_WARNING << "[Skin] " << line << ": div_x is 0 (Line " << line << ")";
        d.div_x = 1;
    }
    if (d.div_y == 0)
    {
        LOG_WARNING << "[Skin] " << line << ": div_y is 0 (Line " << line << ")";
        d.div_y = 1;
    }

    if (d.x == -1 && d.y == -1)
    {
        d.x = d.y = 0;
    }

}

////////////////////////////////////////////////////////////////////////////////
// File parsing
#pragma region 

int SkinLR2::IMAGE()
{
    if (tokensBuf[0] == "#IMAGE")
    {
        const auto& p = tokensBuf[1];
        Path path(p);
        if (path.stem() == "*")
        {
            // Check if the wildcard path is specified by custom settings
            for (const auto& cf : customFile)
            {
                if (cf.filepath == p)
                {
                    const auto& paths = cf.pathList;
                    if (paths.empty())
                        _texNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(""));
                    else
                        _texNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(paths[cf.value].string().c_str()));
                    LOG_DEBUG << "[Skin] " << line << ": Added IMAGE[" << imageCount << "]: " << cf.filepath;
                    ++imageCount;
                    return 2;
                }
            }

            // Or, randomly choose a file
            auto ls = findFiles(path);
            if (!ls.empty())
            {
                size_t ranidx = std::rand() % ls.size();
                _texNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(ls[ranidx].string().c_str()));
                LOG_DEBUG << "[Skin] " << line << ": Added random IMAGE[" << imageCount << "]: " << ls[ranidx].string();
            }
            else
            {
                _texNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(""));
                //imagePath.push_back(defs::file::errorTextureImage);
                LOG_DEBUG << "[Skin] " << line << ": Added random IMAGE[" << imageCount << "]: " << "(file not found)";
            }
            ++imageCount;
            return 3;
        }
        else
        {
            // Normal path
            _texNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(path.string().c_str()));
            LOG_DEBUG << "[Skin] " << line << ": Added IMAGE[" << imageCount << "]: " << path.string();
        }
        ++imageCount;
        return 1;
    }
    return 0;
}

int SkinLR2::FONT()
{
	// TODO load LR2FONT
    if (tokensBuf[0] == "#LR2FONT")
    {
        Path path(tokensBuf[1]);
        //lr2fontPath.push_back(std::move(path));
        LOG_DEBUG << "[Skin] " << line << ": Skipped LR2FONT: " << path.string();
        return 1;
    }
    return 0;
}

int SkinLR2::SYSTEMFONT()
{
	// Could not get system font file path in a reliable way while cross-platforming..
    if (tokensBuf[0] == "#FONT")
    {
        int ptsize = stoine(tokensBuf[1]);
        int thick = stoine(tokensBuf[2]);
        int fonttype = stoine(tokensBuf[3]);
        //StringContent name = tokensBuf[4];
#if defined _WIN32
		TCHAR windir[MAX_PATH];
		GetWindowsDirectory(windir, MAX_PATH);
		StringContent name = windir;
		name += "\\Fonts\\msgothic.ttc";
#elif defined LINUX
		StringContent name = "/usr/share/fonts/tbd.ttf"
#endif
        size_t idx = _fontNameMap.size();
        _fontNameMap[std::to_string(idx)] = std::make_shared<TTFFont>(name.c_str(), ptsize);
        LOG_DEBUG << "[Skin] " << line << ": Added FONT[" << idx << "]: " << name;
    }
    return 0;
}

int SkinLR2::INCLUDE()
{
    if (tokensBuf[0] == "#INCLUDE")
    {
        Path path(tokensBuf[1]);
        auto line = this->line;
        this->line = 0;
        LOG_DEBUG << "[Skin] " << line << ": INCLUDE: " << path.string();
        //auto subCsv = SkinLR2(path);
        //if (subCsv._loaded)
        //    _csvIncluded.push_back(std::move(subCsv));
        loadCSV(path);
        LOG_DEBUG << "[Skin] " << line << ": INCLUDE END //" << path.string();
        this->line = line;
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
    if (tokensBuf[0] == "#STARTINPUT")
    {
        info.timeIntro = stoine(tokensBuf[1]);
        if (info.mode == eMode::RESULT || info.mode == eMode::COURSE_RESULT)
        {
            int rank = stoine(tokensBuf[2]);
            int update = stoine(tokensBuf[3]);
            //if (rank > 0) info.resultStartInputTimeRank = rank;
            //if (update > 0) info.resultStartInputTimeUpdate = update;
            LOG_DEBUG << "[Skin] " << line << ": Skipped STARTINPUT " << rank << " " << update;
        }

        return 1;
    }

    else if (tokensBuf[0] == "#SKIP")
    {
        int time = stoine(tokensBuf[1]);
        info.timeIntro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set Intro freeze time: " << time;
        return 2;
    }

    else if (tokensBuf[0] == "#LOADSTART")
    {
        int time = stoine(tokensBuf[1]);
        info.timeStartLoading = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown before loading: " << time;
        return 3;
    }

    else if (tokensBuf[0] == "#LOADEND")
    {
        int time = stoine(tokensBuf[1]);
        info.timeMinimumLoad = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown after loading: " << time;
        return 4;
    }

    else if (tokensBuf[0] == "#PLAYSTART")
    {
        int time = stoine(tokensBuf[1]);
        info.timeGetReady = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time READY after loading: " << time;
        return 5;
    }

    else if (tokensBuf[0] == "#CLOSE")
    {
        int time = stoine(tokensBuf[1]);
        info.timeFailed = time;
        LOG_DEBUG << "[Skin] " << line << ": Set FAILED time length: " << time;
        return 6;
    }

    else if (tokensBuf[0] == "#FADEOUT")
    {
        int time = stoine(tokensBuf[1]);
        info.timeOutro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set fadeout time length: " << time;
        return 7;
    }

    return 0;
}

int SkinLR2::others()
{
    if (tokensBuf[0] == "#RELOADBANNER")
    {
        reloadBanner = true;
        LOG_DEBUG << "[Skin] " << line << ": Set dynamic banner loading";
        return 1;
    }
    if (tokensBuf[0] == "#TRANSCOLOR")
    {
        int r, g, b;
        r = stoine(tokensBuf[1]);
        g = stoine(tokensBuf[2]);
        b = stoine(tokensBuf[3]);
        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        transColor = {
            static_cast<unsigned>(r),
            static_cast<unsigned>(g),
            static_cast<unsigned>(b)
        };
        LOG_DEBUG << "[Skin] " << line << ": Set transparent color: " << std::hex << r << ' ' << g << ' ' << b << ", but not implemented" << std::dec;
        return 2;
    }
    if (tokensBuf[0] == "#FLIPSIDE")
    {
        flipSide = true;
        return 3;
    }
    if (tokensBuf[0] == "#FLIPRESULT")
    {
        flipResult = true;
        return 4;
    }
    if (tokensBuf[0] == "#DISABLEFLIP")
    {
        disableFlipResult = true;
        return 5;
    }
    if (tokensBuf[0] == "#SCRATCH")
    {
        int a, b;
        a = !!stoine(tokensBuf[1]);
        b = !!stoine(tokensBuf[2]);
        scratchSide1P = a;
        scratchSide2P = b;
        return 6;
    }
    return 0;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Sprite parsing
#pragma region


std::map<Token, LoadLR2SrcFunc> SkinLR2::__src_supported
{
    {"#SRC_IMAGE",      std::bind(&SkinLR2::SRC_IMAGE,      _1)},
    {"#SRC_JUDGELINE",  std::bind(&SkinLR2::SRC_JUDGELINE,  _1)},
    {"#SRC_NUMBER",     std::bind(&SkinLR2::SRC_NUMBER,     _1)},
    {"#SRC_SLIDER",     std::bind(&SkinLR2::SRC_SLIDER,     _1)},
    {"#SRC_BARGRAPH",   std::bind(&SkinLR2::SRC_BARGRAPH,   _1)},
    {"#SRC_BUTTON",     std::bind(&SkinLR2::SRC_BUTTON,     _1)},
    //{"#SRC_ONMOUSE",  std::bind(&SkinLR2::SRC_ONMOUSE, _1)},
    {"#SRC_GROOVEGAUGE",std::bind(&SkinLR2::SRC_GROOVEGAUGE,_1)},
    {"#SRC_TEXT",       std::bind(&SkinLR2::SRC_TEXT,       _1)},
    {"#SRC_NOWJUDGE_1P",std::bind(&SkinLR2::SRC_NOWJUDGE1,  _1)},
    {"#SRC_NOWJUDGE_2P",std::bind(&SkinLR2::SRC_NOWJUDGE2,  _1)},
    {"#SRC_NOWCOMBO_1P",std::bind(&SkinLR2::SRC_NOWCOMBO1,  _1)},
    {"#SRC_NOWCOMBO_2P",std::bind(&SkinLR2::SRC_NOWCOMBO2,  _1)},
};

int SkinLR2::SRC()
{
    auto opt = tokensBuf[0];

    // skip unsupported
	if (__src_supported.find(opt) == __src_supported.end())
		return 0;

    if (opt == "#SRC_TEXT")
    {
        __src_supported[opt](this);
        return 7;
    }

    if (tokensBuf.size() < 11)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // Find texture from map by gr
    std::string gr_key = std::to_string(stoine(tokensBuf[2]));
    textureBuf = (_texNameMap.find(gr_key) != _texNameMap.end()) ? _texNameMap[gr_key] : _texNameMap["Error"];

	__src_supported[opt](this);

    return 1;
}

ParseRet SkinLR2::SRC_IMAGE()
{
	if (tokensBuf.size() < 11)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    refineRect(d, textureBuf->getRect(), line);

	_sprites.push_back(std::make_shared<SpriteAnimated>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setLine(line);
	
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_JUDGELINE()
{
    if (tokensBuf.size() < 11)
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
    }

    lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)& d);
    refineRect(d, textureBuf->getRect(), line);

    gSprites[GLOBAL_SPRITE_IDX_JUDGELINE] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[GLOBAL_SPRITE_IDX_JUDGELINE]->setLine(line);

    auto p = std::make_shared<SpriteGlobal>(GLOBAL_SPRITE_IDX_JUDGELINE);
    _sprites.push_back(p);
    _sprites_child.push_back(p);
    _sprites.back()->setLine(line);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NUMBER()
{
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_number d;
	convertLine(tokensBuf, (int*)&d, 0, 13);
    refineRect(d, textureBuf->getRect(), line);

	// TODO convert num
	eNumber iNum = (eNumber)d.num;

	// get NumType from div_x, div_y
	unsigned f = d.div_y * d.div_x;
	if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
	else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
	else if (f % NumberType::NUM_TYPE_FULL == 0) f = f / NumberType::NUM_TYPE_FULL;
	else f = 0;

	_sprites.emplace_back(std::make_shared<SpriteNumber>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (eTimer)d.timer, f));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setLine(line);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWJUDGE(size_t idx)
{
    if (tokensBuf.size() < 11)
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
    }

    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << line << ": Nowjudge idx out of range (Line " << line << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)& d);
    refineRect(d, textureBuf->getRect(), line);

    gSprites[idx] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[idx]->setLine(line);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWCOMBO(size_t idx)
{
    if (tokensBuf.size() < 14)
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
    }

    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << line << ": Nowjudge idx out of range (Line " << line << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_number d;
    convertLine(tokensBuf, (int*)& d, 0, 13);
    refineRect(d, textureBuf->getRect(), line);

    // TODO convert num
    eNumber iNum = (eNumber)d.num;

    // get NumType from div_x, div_y
    unsigned f = d.div_y * d.div_x;
    if (f % NumberType::NUM_TYPE_NORMAL == 0) f = f / NumberType::NUM_TYPE_NORMAL;
    else if (f % NumberType::NUM_TYPE_BLANKZERO == 0) f = f / NumberType::NUM_TYPE_BLANKZERO;
    else if (f % NumberType::NUM_TYPE_FULL == 0) f = f / NumberType::NUM_TYPE_FULL;
    else f = 0;

    gSprites[idx] = std::make_shared<SpriteNumber>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, (eTimer)d.timer, f);
    gSprites[idx]->setLine(line);
    std::reinterpret_pointer_cast<SpriteNumber>(gSprites[idx])->setInhibitZero(true);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_SLIDER()
{
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_slider d;
	convertLine(tokensBuf, (int*)&d, 0, 13); // 14th: mouse_disable is ignored for now
	
    refineRect(d, textureBuf->getRect(), line);

	_sprites.push_back(std::make_shared<SpriteSlider>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), (SliderDirection)d.muki, d.range, d.div_y*d.div_x, d.cycle, (eSlider)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setLine(line);
	
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BARGRAPH()
{
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_bargraph d;
	convertLine(tokensBuf, (int*)&d, 0, 12);
    refineRect(d, textureBuf->getRect(), line);

	_sprites.push_back(std::make_shared<SpriteBargraph>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), (BargraphDirection)d.muki, d.div_y*d.div_x, d.cycle, (eBargraph)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setLine(line);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BUTTON()
{
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_button d;
	convertLine(tokensBuf, (int*)&d, 0, 13);
    refineRect(d, textureBuf->getRect(), line);
	
	if (d.type < (int)buttonAdapter.size())
	{
		if (auto sw = std::get_if<eSwitch>(&buttonAdapter[d.type]))
		{
			if (*sw == eSwitch::_TRUE)
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
			auto s = std::make_shared<SpriteOption>(
				textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, false, d.div_y, d.div_x);
			s->setInd(SpriteOption::opType::SWITCH, (unsigned)*sw);
			_sprites.push_back(s);
            _sprites_child.push_back(_sprites.back());
            _sprites.back()->setLine(line);
		}
		else if (auto op = std::get_if<eOption>(&buttonAdapter[d.type]))
		{
			auto s = std::make_shared<SpriteOption>(
				textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, false, d.div_y, d.div_x);
			s->setInd(SpriteOption::opType::OPTION, (unsigned)*op);
			_sprites.push_back(s);
            _sprites_child.push_back(_sprites.back());
            _sprites.back()->setLine(line);
		}
	}

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_GROOVEGAUGE()
{
	if (tokensBuf.size() < 13)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_groovegauge d;
	convertLine(tokensBuf, (int*)&d, 0, 12);
    refineRect(d, textureBuf->getRect(), line);

	if (d.div_y * d.div_x < 4)
	{
		LOG_WARNING << "[Skin] " << line << ": div not enough (Line " << line << ")";
        return ParseRet::DIV_NOT_ENOUGH;
	}

    size_t idx = d._null == 0 ? GLOBAL_SPRITE_IDX_1PGAUGE : GLOBAL_SPRITE_IDX_2PGAUGE;
    eNumber en = d._null == 0 ? eNumber::PLAY_1P_GROOVEGAUGE : eNumber::PLAY_2P_GROOVEGAUGE;

    gSprites[idx] = std::make_shared<SpriteGaugeGrid>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x / 4, d.cycle, d.add_x, d.add_y, 0, 100, 50,
        (eTimer)d.timer, en, d.div_y, d.div_x);
    gSprites[idx]->setLine(line);

    auto p = std::make_shared<SpriteGlobal>(idx);
    _sprites.push_back(p);
    _sprites_child.push_back(p);
    _sprites.back()->setLine(line);
	
    return ParseRet::OK;
}


ParseRet SkinLR2::SRC_TEXT()
{
	if (tokensBuf.size() < 5)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_text d;
	convertLine(tokensBuf, (int*)&d, 0, 4);

	_sprites.push_back(std::make_shared<SpriteText>(
		_fontNameMap[std::to_string(d.font)], (eText)d.st, (TextAlign)d.align));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setLine(line);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWJUDGE1()
{
	if (tokensBuf.size() < 12)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}
	bufJudge1PSlot = stoine(tokensBuf[1]);
    noshiftJudge1P[bufJudge1PSlot] = stoine(tokensBuf[11]);
    if (bufJudge1PSlot >= 0 && bufJudge1PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_1PJUDGE + bufJudge1PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setLine(line);
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
	if (tokensBuf.size() < 12)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}
	bufJudge2PSlot = stoine(tokensBuf[1]);
    noshiftJudge2P[bufJudge2PSlot] = stoine(tokensBuf[11]);
    if (bufJudge2PSlot >= 0 && bufJudge2PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_2PJUDGE + bufJudge2PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setLine(line);
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
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}
    bufJudge1PSlot = stoine(tokensBuf[1]);
	tokensBuf[11] = std::to_string((int)eNumber::_DISP_NOWCOMBO_1P);
    switch (stoine(tokensBuf[12]))
    {
    case 0: tokensBuf[12] = "1"; break;
    case 1: tokensBuf[12] = "2"; break;
    case 2:
    default:tokensBuf[12] = "0"; break;
    }
    if (bufJudge1PSlot >= 0 && bufJudge1PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_1PJUDGENUM + bufJudge1PSlot;
        auto ret = SRC_NOWCOMBO(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setLine(line);
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
	if (tokensBuf.size() < 14)
	{
		LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        return ParseRet::PARAM_NOT_ENOUGH;
	}
    bufJudge2PSlot = stoine(tokensBuf[1]);
	tokensBuf[11] = std::to_string((int)eNumber::_DISP_NOWCOMBO_2P);
    switch (stoine(tokensBuf[12]))
    {
    case 0: tokensBuf[12] = "1"; break;
    case 1: tokensBuf[12] = "2"; break;
    case 2: 
    default:tokensBuf[12] = "0"; break;
    }
    if (bufJudge2PSlot >= 0 && bufJudge2PSlot < 6)
	{
        size_t idx = GLOBAL_SPRITE_IDX_2PJUDGENUM + bufJudge2PSlot;
        auto ret = SRC_NOWCOMBO(idx);
        if (ret == ParseRet::OK)
        {
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setLine(line);
        }
        else
        {
            return ret;
        }
	}
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOTE()
{
    // skip unsupported
    if ( !(tokensBuf[0] == "#SRC_NOTE" || tokensBuf[0] == "#SRC_MINE" || tokensBuf[0] == "#SRC_LN_END" || tokensBuf[0] == "#SRC_LN_BODY"
        || tokensBuf[0] == "#SRC_LN_START"|| tokensBuf[0] == "#SRC_AUTO_NOTE" || tokensBuf[0] == "#SRC_AUTO_MINE" || tokensBuf[0] == "#SRC_AUTO_LN_END" 
        || tokensBuf[0] == "#SRC_AUTO_LN_BODY" || tokensBuf[0] == "#SRC_AUTO_LN_START" || tokensBuf[0] == "#SRC_LINE"))
    {
        return ParseRet::OK;
    }

    // load line into data struct
	lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)&d);

    // TODO convert timer
    eTimer iTimer = (eTimer)d.timer;

    // Find texture from map by gr
    pTexture tex = nullptr;
    std::string gr_key = std::to_string(d.gr);
    if (_texNameMap.find(gr_key) != _texNameMap.end())
    {
        tex = _texNameMap[gr_key];
    }
    else
    {
        tex = _texNameMap["Error"];
    }

    refineRect(d, textureBuf->getRect(), line);

    // SRC
    if (tokensBuf.size() < 11)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    NoteChannelCategory cat = NoteChannelCategory::_;
    NoteChannelIndex idx = NoteChannelIndex::_;
    int ret = 0;

    // SRC_NOTE
    if (tokensBuf[0] == "#SRC_NOTE")
    {
        cat = NoteChannelCategory::Note;
        idx = (NoteChannelIndex)d._null;
        ret = 1;
    }
    else if (tokensBuf[0] == "#SRC_LINE")
    {
        cat = NoteChannelCategory::BARLINE;
        idx = NOTECHANNEL_BARLINE;
        ret = 2;
    }
    else
    {
        // other types not supported
        ret = 0;
    }

    size_t i = channelToIdx(cat, idx);
    if (i == CHANNEL_INVALID)
    {
        LOG_WARNING << "[Skin] Note channel illegal: " << unsigned(cat) << ", " << unsigned(idx);
        return ParseRet::PARAM_INVALID;
    }

    _sprites.push_back(std::make_shared<SpriteLaneVertical>(
        _texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());

    _laneSprites[i] = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
    _laneSprites[i]->setChannel(cat, idx);
    //LOG_DEBUG << "[Skin] " << line << ": Set Note " << idx << " sprite (texture: " << gr_key << ", timer: " << d.timer << ")";

    _laneSprites[i]->pNote->appendKeyFrame({ 0, {Rect(),
        RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0 } });
    _laneSprites[i]->pNote->setLoopTime(0);
    return ParseRet::OK;
}

static std::map<Token, int> __dst_supported
{
	{"#DST_IMAGE",1},
	{"#DST_NUMBER",2},
	{"#DST_SLIDER",3},
	{"#DST_BARGRAPH",4},
	{"#DST_BUTTON",5},
	{"#DST_ONMOUSE",6},
	{"#DST_TEXT",7},
	{"#DST_JUDGELINE",8},
	{"#DST_GROOVEGAUGE",9},
	{"#DST_NOWJUDGE_1P",10},
	{"#DST_NOWCOMBO_1P",11},
	{"#DST_NOWJUDGE_2P",12},
	{"#DST_NOWCOMBO_2P",13},
};
int SkinLR2::DST()
{
    auto opt = tokensBuf[0];

    if (__dst_supported.find(opt) == __dst_supported.end() || _sprites.empty())
        return 0;

    if (tokensBuf.size() < 14)
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
    }

    // load line into data struct
    int src[32]{ 0 };
    convertLine(tokensBuf, src, 0, 14);
    lr2skin::dst& d = *(lr2skin::dst*)src;

    int ret = 0;
    auto e = _sprites.back();
    if (e && e->type() == SpriteTypes::GLOBAL)
    {
        auto ee = e;
        do
        {
            ee = gSprites[std::reinterpret_pointer_cast<SpriteGlobal>(ee)->get()];
            std::reinterpret_pointer_cast<SpriteGlobal>(e)->set(ee);

            if (ee == nullptr)
            {
                LOG_WARNING << "[Skin] " << line << ": Previous src definition invalid (Line: " << line << ")";
                return 0;
            }

        } while (ee->type() == SpriteTypes::GLOBAL);
    }

    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << line << ": Previous src definition invalid (Line: " << line << ")";
        return 0;
    }

	ret = __dst_supported[opt];

    if (e->isKeyFrameEmpty())
    {
        convertLine(tokensBuf, src, 14, 16);
        //lr2skin::dst& d = *(lr2skin::dst*)src;
        if (tokensBuf.size() < 17 || tokensBuf[16].empty())
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            d.loop = -1;
            d.timer = 0;
            d.op[0] = d.op[1] = d.op[2] = d.op[3] = DST_TRUE;
        }
        else
        {
			convertLine(tokensBuf, src, 16, 17);
            for (size_t i = 0; i < 4; ++i)
            {
                StringContent ops = tokensBuf[18 + i];
                if (ops[0] == '!' || ops[0] == '-')
                    *(int*)&d.op[i] = -stoine(ops.substr(1));
                else
                    *(int*)&d.op[i] = stoine(ops);
            }
        }

		if (opt == "#DST_NOWJUDGE_1P" || opt == "#DST_NOWCOMBO_1P" )
		{
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
		}
		else if (opt == "#DST_NOWJUDGE_2P" || opt == "#DST_NOWCOMBO_2P" )
		{
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
		}

        drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
        e->setLoopTime(d.loop);
		e->setTrigTimer((eTimer)d.timer);
        if (d.time > 0)
        {
            LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
            e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), RenderParams::accTy::DISCONTINOUS, Color(d.r, d.g, d.b, 0),
				(BlendMode)d.blend, !!d.filter, (double)d.angle } });
        }
    }

    e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
		(BlendMode)d.blend, !!d.filter, (double)d.angle } });
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << line << ": Set sprite Keyframe (time: " << d.time << ")";

    return ret;
}


ParseRet SkinLR2::DST_NOTE()
{
    if (tokensBuf[0] != "#DST_NOTE")
        return ParseRet::OK;

    if (tokensBuf.size() < 14)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // load line into data struct
    int src[32]{ 0 };
    convertLine(tokensBuf, src, 0, 14);
    lr2skin::dst& d = *(lr2skin::dst*)src;
	NoteChannelIndex idx = (NoteChannelIndex)d._null;

	for (size_t i = (size_t)NoteChannelCategory::Note; i < (size_t)NoteChannelCategory::NOTECATEGORY_COUNT; ++i)
	{
		NoteChannelCategory cat = (NoteChannelCategory)i;
		int ret = 0;
		auto e = _laneSprites[channelToIdx(cat, idx)];
		if (e == nullptr)
		{
			LOG_WARNING << "[Skin] " << line << ": Note SRC definition invalid " <<
				"(Type: " << i << ", Index: " << idx << " ) " <<
				"(Line: " << line << ")";
			continue;
		}

		if (e->type() != SpriteTypes::NOTE_VERT)
		{
			LOG_WARNING << "[Skin] " << line << ": Note SRC definition is not NOTE " <<
				"(Type: " << i << ", Index: " << idx << " ) " <<
				"(Line: " << line << ")";
			continue;
		}

		if (!e->isKeyFrameEmpty())
		{
			LOG_WARNING << "[Skin] " << line << ": Note DST is already defined " << 
				"(Type: " << i << ", Index: " << idx << " ) " <<
				"(Line: " << line << ")";
			e->clearKeyFrames();
		}
		e->pNote->clearKeyFrames();
		e->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
			(BlendMode)d.blend, !!d.filter, (double)d.angle } });

		// set sprite channel
		auto p = std::static_pointer_cast<SpriteLaneVertical>(e);

        p->_playerSlot = d._null / 10;  // player slot, 1P:0, 2P:1

		// refine rect: x=dst_x, y=-dst_h, w=dst_w, h=dst_y
		int dst_h;
		//p->getRectSize(d.w, dummy);
		dst_h = d.h;
		d.h = d.y;
		d.y = -dst_h;

		convertLine(tokensBuf, src, 14, 16);
		//lr2skin::dst& d = *(lr2skin::dst*)src;
		if (tokensBuf.size() < 17 || tokensBuf[16].empty())
		{
			LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
			d.loop = -1;
			d.timer = 0;
			d.op[0] = d.op[1] = d.op[2] = d.op[3] = DST_TRUE;
		}
		else
		{
			for (size_t i = 0; i < 4; ++i)
			{
				StringContent ops = tokensBuf[18 + i];
				if (ops[0] == '!' || ops[0] == '-')
					*(int*)&d.op[i] = -stoine(ops.substr(1));
				else
					*(int*)&d.op[i] = stoine(ops);
			}
		}

		drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
		e->setLoopTime(d.loop);
		if (d.time > 0)
		{
			LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
			e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), RenderParams::accTy::DISCONTINOUS, Color(d.r, d.g, d.b, 0),
				BlendMode::NONE, false, (double)d.angle } });
		}
		e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
			(BlendMode)d.blend, !!d.filter, (double)d.angle } });
		e->setLoopTime(0);
		//e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
		//LOG_DEBUG << "[Skin] " << line << ": Set Lane sprite Keyframe (time: " << d.time << ")";
	}

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_LINE()
{
    if (tokensBuf[0] != "#DST_LINE")
        return ParseRet::OK;

    if (tokensBuf.size() < 14)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // load line into data struct
    int src[32]{ 0 };
    convertLine(tokensBuf, src, 0, 14);
    lr2skin::dst& d = *(lr2skin::dst*)src;

    int ret = 0;
    auto e = _sprites.back();
    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << line << ": Barline SRC definition invalid " <<
            "(Line: " << line << ")";
        return ParseRet::SRC_DEF_INVALID;
    }

    if (e->type() != SpriteTypes::NOTE_VERT)
    {
        LOG_WARNING << "[Skin] " << line << ": Barline SRC definition is not NOTE " <<
            "(Line: " << line << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }
    if (!e->isKeyFrameEmpty())
    {
        LOG_WARNING << "[Skin] " << line << ": Barline DST is already defined " <<
            "(Line: " << line << ")";
        e->clearKeyFrames();
    }

    // set sprite channel
    auto p = std::static_pointer_cast<SpriteLaneVertical>(e);

    p->_playerSlot = d._null / 10;  // player slot, 1P:0, 2P:1

    NoteChannelCategory cat = p->getChannelCat();
    NoteChannelIndex idx = p->getChannelIdx();
    if (cat != NoteChannelCategory::BARLINE || idx != NOTECHANNEL_BARLINE)
    {
        LOG_WARNING << "[Skin] " << line << ": Previous SRC definition is not LINE " <<
            "(Line: " << line << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }

    p->pNote->clearKeyFrames();
    p->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle } });

    // refine rect: x=dst_x, y=-dst_h, w=dst_w, h=dst_y
    int dst_h;
    //p->getRectSize(d.w, dummy);
    dst_h = d.h;
    d.h = d.y;
    d.y = -dst_h;

    convertLine(tokensBuf, src, 14, 16);
    //lr2skin::dst& d = *(lr2skin::dst*)src;
    if (tokensBuf.size() < 17 || tokensBuf[16].empty())
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        d.loop = -1;
        d.timer = 0;
        d.op[0] = d.op[1] = d.op[2] = d.op[3] = DST_TRUE;
    }
    else
    {
        for (size_t i = 0; i < 4; ++i)
        {
            StringContent ops = tokensBuf[18 + i];
            if (ops[0] == '!' || ops[0] == '-')
                * (int*)& d.op[i] = -stoine(ops.substr(1));
            else
                *(int*)& d.op[i] = stoine(ops);
        }
    }

    drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
    e->setLoopTime(d.loop);
    if (d.time > 0)
    {
        LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
        e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), RenderParams::accTy::DISCONTINOUS, Color(d.r, d.g, d.b, 0),
            BlendMode::NONE, false, (double)d.angle } });
    }
    e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle } });
    e->setLoopTime(0);
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << line << ": Set Lane sprite (Barline) Keyframe (time: " << d.time << ")";

    return ParseRet::OK;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Dispatcher
#pragma region

int SkinLR2::parseLine(const Tokens &raw)
{
    tokensBuf.assign(30, "");
    for (size_t idx = 0; idx < raw.size(); ++idx)
        tokensBuf[idx] = raw[idx];
    try {
        if (IMAGE())
            return 1;
        if (FONT())
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
        if (SRC_NOTE() == ParseRet::OK)
            return 9;
        if (DST_NOTE() == ParseRet::OK)
            return 10;
        if (DST_LINE() == ParseRet::OK)
            return 11;
    }
    catch (std::invalid_argument e)
    {
        LOG_WARNING << "[Skin] " << line << ": Invalid Argument: " << "(Line " << line << ")";
    }
    catch (std::out_of_range e)
    {
        LOG_WARNING << "[Skin] " << line << ": Out of range: " << "(Line " << line << ")";
    }
    return 0;
}

void SkinLR2::IF(const Tokens &t, std::ifstream& lr2skin)
{
    bool optSwitch = true;
    if (t[0] != "#ELSE")
    {
        LOG_WARNING << "[Skin] " << line << ": No IF parameters " << " (Line " << line << ")";
    }
    for (auto it = ++t.begin(); it != t.end(); ++it)
    {
        auto [idx, val] = stoub(*it);
        if (idx == -1)
        {
            LOG_WARNING << "[Skin] " << line << ": Invalid DST_OPTION Index, deal as false (Line " << line << ")";
            optSwitch = false;
            break;
        }
        bool dst = getDstOpt((dst_option)idx);
        if (val) dst = !dst;
        optSwitch = optSwitch && dst;
    }

    if (optSwitch)
    {
        while (!lr2skin.eof())
        {
            auto tokens = csvNextLineTokenize(lr2skin);
            if (tokens.empty()) continue;

            if (*tokens.begin() == "#ELSE" || *tokens.begin() == "#ELIF")
            {
                while (!lr2skin.eof() && *tokens.begin() != "#ENDIF")
                {
                    tokens = csvNextLineTokenize(lr2skin);
                    if (tokens.begin() == tokens.end()) continue;
                }
                return;
            }
            else if (*tokens.begin() == "#ENDIF")
                return;
            else
                parseLine(tokens);
        }
    }
    else
    {
        while (!lr2skin.eof())
        {
            auto tokens = csvNextLineTokenize(lr2skin);
            if (tokens.begin() == tokens.end()) continue;

            if (*tokens.begin() == "#ELSE")
            {
                IF(tokens, lr2skin);
                return;
            }
            else if (*tokens.begin() == "#ELSEIF")
            {
                IF(tokens, lr2skin);
                return;
            }
            else if (*tokens.begin() == "#ENDIF")
                return;
        }
    }
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

int SkinLR2::HEADER()
{
    if (tokensBuf[0] == "#INFORMATION")
    {
        int type = stoine(tokensBuf[1]);
        StringContent title = tokensBuf[2];
        StringContent maker = tokensBuf[3];
        Path thumbnail(tokensBuf[4]);

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

        _texNameMap["THUMBNAIL"] = std::make_shared<Texture>(Image(thumbnail.string().c_str()));
        if (_texNameMap["THUMBNAIL"] == nullptr)
            LOG_WARNING << "[Skin] " << line << ": thumbnail loading failed: " << thumbnail.string() << " (Line " << line << ")";

        LOG_DEBUG << "[Skin] " << line << ": Loaded metadata: " << title << " | " << maker;

        return 1;
    }

    else if (tokensBuf[0] == "#CUSTOMOPTION")
    {
        StringContent title = tokensBuf[1];
        int dst_op = stoine(tokensBuf[2]);
        if (dst_op < 900 || dst_op > 999)
        {
            LOG_WARNING << "[Skin] " << line << ": Invalid option value: " << dst_op << " (Line " << line << ")";
            return -2;
        }
        Tokens op_label;
        for (size_t idx = 3; idx < tokensBuf.size() && !tokensBuf[idx].empty(); ++idx)
            op_label.push_back(tokensBuf[idx]);

        LOG_DEBUG << "[Skin] " << line << ": Loaded Custom option " << title << ": " << dst_op;
        customize.push_back({ (unsigned)dst_op, title, std::move(op_label), 0 });
        return 2;
    }

    else if (tokensBuf[0] == "#CUSTOMFILE")
    {
        StringContent title = tokensBuf[1];
        StringContent p = tokensBuf[2];
        Path pathf(p);
        Path def(tokensBuf[3]);

        auto ls = findFiles(pathf);
        unsigned defVal = 0;
        for (size_t param = 0; param < ls.size(); ++param)
            if (ls[param].stem() == def)
            {
                defVal = param;
                break;
            }

        LOG_DEBUG << "[Skin] " << line << ": Loaded Custom file " << title << ": " << pathf.string();
        customFile.push_back({ title, p, std::move(ls), defVal, defVal });
        return 3;
    }

    else if (tokensBuf[0] == "#ENDOFHEADER")
    {
        return -1;
    }

    return 0;
}

#pragma endregion

SkinLR2::SkinLR2(Path p)
{
	_laneSprites.resize(CHANNEL_COUNT);
    loadCSV(p);
}

void SkinLR2::loadCSV(Path p)
{
    std::ifstream lr2skin(p, std::ios::binary);
    if (!lr2skin.is_open())
    {
        LOG_ERROR << "[Skin] " << line << ": Skin File Not Found: " << std::filesystem::absolute(p).string();
        return;
    }

    while (!lr2skin.eof())
    {
        tokensBuf = csvNextLineTokenize(lr2skin);
        if (tokensBuf.begin() == tokensBuf.end()) continue;

        if (HEADER() == -1)
            break;
    }
    LOG_DEBUG << "[Skin] " << line << ": Header loading finished";

    if (lr2skin.eof())
    {
        // reset position to head
        lr2skin.close();
        lr2skin.open(p, std::ios::binary);
    }

    // TODO load skin customize
    for (auto c : customize)
    {
        //data().setDstOption(static_cast<dst_option>(c.dst_op + c.value), true);
        setCustomDstOpt(c.dst_op, c.value, true);
    }

    // Add extra textures

    while (!lr2skin.eof())
    {
        auto tokens = csvNextLineTokenize(lr2skin);
        if (tokens.begin() == tokens.end()) continue;

        if (*tokens.begin() == "#IF")
            IF(tokens, lr2skin);
        else
            parseLine(tokens);
    }


    LOG_DEBUG << "[Skin] " << line << ": Loaded " << p.string();
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

}


//////////////////////////////////////////////////

void SkinLR2::update()
{
    // update sprites
    vSkin::update();

	// update op
	updateDstOpt();

#ifndef _DEBUG
	std::for_each(std::execution::par_unseq, drawQueue.begin(), drawQueue.end(), [](auto& e)
#else
	for (auto& e : drawQueue)
#endif
	{
        e.draw = getDstOpt(e.op1) && getDstOpt(e.op2) && getDstOpt(e.op3);
	}
#ifndef _DEBUG
	);
#endif

	// update nowjudge/nowcombo
	// TODO
    for (size_t i = 0; i < 6; ++i)
    {
        if (gSprites[i] && gSprites[i + 6] && gSprites[i]->_draw && gSprites[i + 6]->_draw)
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 6]);
            Rect delta{ 0,0,0,0 };
            //delta.x = int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            delta.x += judge->_current.rect.x;
            delta.y += judge->_current.rect.y;
            if (!noshiftJudge1P[i])
            {
                judge->_current.rect.x -= int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            }
            for (auto& d : combo->_rects)
            {
                d.x += delta.x;
                d.y += delta.y;
            }
        }
        if (gSprites[i + 12] && gSprites[i + 18] && gSprites[i + 12]->_draw && gSprites[i + 18]->_draw)
        {
            std::shared_ptr<SpriteAnimated> judge = std::reinterpret_pointer_cast<SpriteAnimated>(gSprites[i + 12]);
            std::shared_ptr<SpriteNumber> combo = std::reinterpret_pointer_cast<SpriteNumber>(gSprites[i + 18]);
            Rect delta{ 0,0,0,0 };
            //delta.x = int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            delta.x += judge->_current.rect.x;
            delta.y += judge->_current.rect.y;
            if (!noshiftJudge2P[i])
            {
                judge->_current.rect.x -= int(std::floor(0.5 * combo->_current.rect.w * combo->_numDigits));
            }
            for (auto& d : combo->_rects)
            {
                d.x += delta.x;
                d.y += delta.y;
            }
        }
    }

}

void SkinLR2::draw() const
{
    for (auto& e : drawQueue)
    {
        if (e.draw) e.ps->draw();
    }
    //for (auto& c : _csvIncluded)
    //{
    //    c.draw();
    //}
}