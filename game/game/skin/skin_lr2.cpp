#include "skin_lr2.h"
#include <plog/Log.h>
#include "utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <set>
#include <variant>
#include <execution>
#include "game/data/option.h"
#include "game/data/switch.h"
#include "game/graphics/video.h"
#include "game/scene/scene_context.h"

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
        int plusonly;
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
        int _null;      //0
        int time;       //1
        int x, y, w, h; //2, 3, 4, 5
        int acc;        //6
        int a, r, g, b; //7, 8, 9, 10
        int blend;      //11
        int filter;     //12
        int angle;      //13
        int center;     //14
        int loop = -1;  //15
        int timer = 1;  //16    
        dst_option op[4]{DST_TRUE, DST_TRUE, DST_TRUE, DST_TRUE};
    };

}



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

#pragma region LR2 csv parsing

Tokens SkinLR2::csvNextLineTokenize(std::istream& file)
{
    ++srcLine;
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

    const std::regex re{ R"((([^\\,]|\\.)*?)(,|$))" };
    Tokens result = { std::sregex_token_iterator(linecsv.begin(), linecsv.end(), re, 1), std::sregex_token_iterator() };

    // set first token uppercase
    for (auto& c : result[0])
        if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';

    // remove trailing empty tokens
    size_t lastToken;
    for (lastToken = result.size() - 1; lastToken >= 0 && lastToken < result.size() && result[lastToken].empty(); --lastToken);
    result.resize(lastToken + 1);

    return result;
}

size_t convertLine(const Tokens& t, int* pData, size_t start = 0, size_t count = sizeof(lr2skin::s_basic) / sizeof(int))
{
    memset(&pData[start], 0, count * sizeof(int));
    size_t end = start + count;

    size_t i;
    for (i = start; i < end && i < t.size(); ++i)
    {
        pData[i] = stoine(t[i]);
    }
    return i;
}

size_t convertOps(const Tokens& tokensBuf, int* pData, size_t offset = 16, size_t size = 3)
{
    for (int i = 0; i < size; ++i) pData[i] = DST_TRUE;
    if (tokensBuf.size() < offset || offset > 0 && tokensBuf[offset - 1].empty())
    {
        return 0;
    }

    size_t i;
    for (i = 0; i < size && offset + i < tokensBuf.size(); ++i)
    {
        StringContent ops = tokensBuf[offset + i];
        if (ops[0] == '!' || ops[0] == '-')
            pData[i] = -stoine(ops.substr(1));
        else
            pData[i] = stoine(ops);
    }
    return i;

}

void refineRect(lr2skin::s_basic& d, const Rect rect, unsigned srcLine)
{
	if (d.w == -1)
		d.w = rect.w;
	if (d.h == -1)
		d.h = rect.h;

    if (d.div_x == 0)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": div_x is 0";
        d.div_x = 1;
    }
    if (d.div_y == 0)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": div_y is 0";
        d.div_y = 1;
    }

    if (d.x == -1 && d.y == -1)
    {
        d.x = d.y = 0;
    }

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

////////////////////////////////////////////////////////////////////////////////
// File parsing
#pragma region 
int SkinLR2::IMAGE()
{
    if (optBuf == "#IMAGE")
    {
        const auto& p = tokensBuf[0];
        Path path(p);
        if (path.stem() == "*")
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
            for (const auto& cf : customFile)
            {
                if (cf.filepath == p)
                {
                    const auto& paths = cf.pathList;
					if (paths.empty())
					{
                        _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(""));
						LOG_DEBUG << "[Skin] " << srcLine << ": Added IMAGE[" << imageCount << "]: " << "(placeholder)";
					}
					else
					{
						if (video_file_extensions.find(toLower(paths[cf.value].extension().string())) != video_file_extensions.end())
						{
							_vidNameMap[std::to_string(imageCount)] = std::make_shared<sVideo>(paths[cf.value], true);
							_textureNameMap[std::to_string(imageCount)] = _textureNameMap["White"];
						}
						else
                            _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(paths[cf.value].string().c_str()));
						LOG_DEBUG << "[Skin] " << srcLine << ": Added IMAGE[" << imageCount << "]: " << cf.filepath;
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
				LOG_DEBUG << "[Skin] " << srcLine << ": Added random IMAGE[" << imageCount << "]: " << "(file not found)";
			}
            else
			{
				size_t ranidx = std::rand() % ls.size();
				if (video_file_extensions.find(toLower(ls[ranidx].extension().string())) != video_file_extensions.end())
				{
					_vidNameMap[std::to_string(imageCount)] = std::make_shared<sVideo>(ls[ranidx], true);
					_textureNameMap[std::to_string(imageCount)] = _textureNameMap["Error"];
				}
				else
					_textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(ls[ranidx].string().c_str()));
				LOG_DEBUG << "[Skin] " << srcLine << ": Added random IMAGE[" << imageCount << "]: " << ls[ranidx].string();
			}
            ++imageCount;
            return 3;
        }
        else
        {
            // Normal path
            _textureNameMap[std::to_string(imageCount)] = std::make_shared<Texture>(Image(path.string().c_str()));
            LOG_DEBUG << "[Skin] " << srcLine << ": Added IMAGE[" << imageCount << "]: " << path.string();
        }
        ++imageCount;
        return 1;
    }
    return 0;
}

int SkinLR2::FONT()
{
	// TODO load LR2FONT
    if (optBuf == "#LR2FONT")
    {
        Path path(tokensBuf[0]);
        //lr2fontPath.push_back(std::move(path));
        LOG_DEBUG << "[Skin] " << srcLine << ": Skipped LR2FONT: " << path.string();
        return 1;
    }
    return 0;
}

int SkinLR2::SYSTEMFONT()
{
	// Could not get system font file path in a reliable way while cross-platforming..
    if (optBuf == "#FONT")
    {
        int ptsize = stoine(tokensBuf[0]);
        int thick = stoine(tokensBuf[1]);
        int fonttype = stoine(tokensBuf[2]);
        //StringContent name = tokensBuf[3];
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
        LOG_DEBUG << "[Skin] " << srcLine << ": Added FONT[" << idx << "]: " << name;
        return 1;
    }
    return 0;
}

int SkinLR2::INCLUDE()
{
    if (optBuf == "#INCLUDE")
    {
        Path path(tokensBuf[0]);
        auto line_parent = srcLine;
        LOG_DEBUG << "[Skin] " << line_parent << ": INCLUDE: " << path.string();
        //auto subCsv = SkinLR2(path);
        //if (subCsv._loaded)
        //    _csvIncluded.push_back(std::move(subCsv));
        loadCSV(path);
        LOG_DEBUG << "[Skin] " << line_parent << ": INCLUDE END //" << path.string();
        srcLine = line_parent + 1;
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
    if (optBuf == "#STARTINPUT")
    {
        info.timeIntro = stoine(tokensBuf[0]);
        if (info.mode == eMode::RESULT || info.mode == eMode::COURSE_RESULT)
        {
            int rank = stoine(tokensBuf[1]);
            int update = stoine(tokensBuf[2]);
            if (rank > 0) info.timeResultRank = rank;
            if (update > 0) info.timeResultRecord = update;
            LOG_DEBUG << "[Skin] " << srcLine << ": STARTINPUT " << info.timeIntro << " " << rank << " " << update;
        }

        return 1;
    }

    else if (optBuf == "#LOADSTART")
    {
        int time = stoine(tokensBuf[0]);
        info.timeStartLoading = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set time colddown before loading: " << time;
        return 3;
    }

    else if (optBuf == "#LOADEND")
    {
        int time = stoine(tokensBuf[0]);
        info.timeMinimumLoad = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set time colddown after loading: " << time;
        return 4;
    }

    else if (optBuf == "#PLAYSTART")
    {
        int time = stoine(tokensBuf[0]);
        info.timeGetReady = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set time READY after loading: " << time;
        return 5;
    }

    else if (optBuf == "#CLOSE")
    {
        int time = stoine(tokensBuf[0]);
        info.timeFailed = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set FAILED time length: " << time;
        return 6;
    }

    else if (optBuf == "#FADEOUT")
    {
        int time = stoine(tokensBuf[0]);
        info.timeOutro = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set fadeout time length: " << time;
        return 7;
    }

    else if (optBuf == "#SKIP")
    {
        int time = stoine(tokensBuf[0]);
        info.timeDecideSkip = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set Decide skip time: " << time;
        return 8;
    }

    else if (optBuf == "#SCENETIME")
    {
        int time = stoine(tokensBuf[0]);
        info.timeDecideExpiry = time;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set Decide expiry time: " << time;
        return 9;
    }

    return 0;
}

int SkinLR2::others()
{
    if (optBuf == "#RELOADBANNER")
    {
        reloadBanner = true;
        LOG_DEBUG << "[Skin] " << srcLine << ": Set dynamic banner loading";
        return 1;
    }
    if (optBuf == "#TRANSCOLOR")
    {
        int r, g, b;
        r = stoine(tokensBuf[0]);
        g = stoine(tokensBuf[1]);
        b = stoine(tokensBuf[2]);
        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
        transColor = {
            static_cast<unsigned>(r),
            static_cast<unsigned>(g),
            static_cast<unsigned>(b)
        };
        LOG_DEBUG << "[Skin] " << srcLine << ": Set transparent color: " << std::hex << r << ' ' << g << ' ' << b << ", but not implemented" << std::dec;
        return 2;
    }
    if (optBuf == "#FLIPSIDE")
    {
        flipSide = true;
        return 3;
    }
    if (optBuf == "#FLIPRESULT")
    {
        flipResult = true;
        return 4;
    }
    if (optBuf == "#DISABLEFLIP")
    {
        disableFlipResult = true;
        return 5;
    }
    if (optBuf == "#SCRATCH")
    {
        int a, b;
        a = !!stoine(tokensBuf[0]);
        b = !!stoine(tokensBuf[1]);
        scratchSide1P = a;
        scratchSide2P = b;
        return 6;
    }
    if (optBuf == "#BAR_CENTER")
    {
        barCenter = stoine(tokensBuf[0]);
        return 7;
    }
    if (optBuf == "#BAR_AVAILABLE")
    {
        barClickableFrom = stoine(tokensBuf[0]);
        barClickableTo = stoine(tokensBuf[1]);
        return 8;
    }
    return 0;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Sprite parsing
#pragma region


int SkinLR2::SRC()
{
    auto opt = optBuf;

    static const std::map<Token, LoadLR2SrcFunc> __src_supported
    {
        {"#SRC_IMAGE",      std::bind(&SkinLR2::SRC_IMAGE,      _1)},
        {"#SRC_JUDGELINE",  std::bind(&SkinLR2::SRC_JUDGELINE,  _1)},
        {"#SRC_NUMBER",     std::bind(&SkinLR2::SRC_NUMBER,     _1)},
        {"#SRC_SLIDER",     std::bind(&SkinLR2::SRC_SLIDER,     _1)},
        {"#SRC_BARGRAPH",   std::bind(&SkinLR2::SRC_BARGRAPH,   _1)},
        {"#SRC_BUTTON",     std::bind(&SkinLR2::SRC_BUTTON,     _1)},
        {"#SRC_ONMOUSE",    std::bind(&SkinLR2::SRC_ONMOUSE,    _1)},
        {"#SRC_MOUSECURSOR",std::bind(&SkinLR2::SRC_MOUSECURSOR,_1)},
        {"#SRC_GROOVEGAUGE",std::bind(&SkinLR2::SRC_GROOVEGAUGE,_1)},
        {"#SRC_TEXT",       std::bind(&SkinLR2::SRC_TEXT,       _1)},
        {"#SRC_NOWJUDGE_1P",std::bind(&SkinLR2::SRC_NOWJUDGE1,  _1)},
        {"#SRC_NOWJUDGE_2P",std::bind(&SkinLR2::SRC_NOWJUDGE2,  _1)},
        {"#SRC_NOWCOMBO_1P",std::bind(&SkinLR2::SRC_NOWCOMBO1,  _1)},
        {"#SRC_NOWCOMBO_2P",std::bind(&SkinLR2::SRC_NOWCOMBO2,  _1)},
        {"#SRC_BGA",        std::bind(&SkinLR2::SRC_BGA,        _1)},
        {"#SRC_BAR_BODY",   std::bind(&SkinLR2::SRC_BAR_BODY,   _1)},
        {"#SRC_BAR_FLASH",  std::bind(&SkinLR2::SRC_BAR_FLASH,  _1)},
        {"#SRC_BAR_LEVEL",  std::bind(&SkinLR2::SRC_BAR_LEVEL,  _1)},
        {"#SRC_BAR_LAMP",   std::bind(&SkinLR2::SRC_BAR_LAMP,   _1)},
        {"#SRC_BAR_TITLE",  std::bind(&SkinLR2::SRC_BAR_TITLE,  _1)},
        {"#SRC_BAR_RANK",   std::bind(&SkinLR2::SRC_BAR_RANK,   _1)},
        {"#SRC_BAR_RIVAL",  std::bind(&SkinLR2::SRC_BAR_RIVAL,  _1)},
        {"#SRC_BAR_MY_LAMP",  std::bind(&SkinLR2::SRC_BAR_RIVAL_MYLAMP,  _1)},
        {"#SRC_BAR_RIVAL_LAMP",  std::bind(&SkinLR2::SRC_BAR_RIVAL_RIVALLAMP,  _1)},
    };

    // skip unsupported
	if (__src_supported.find(opt) == __src_supported.end())
		return 0;

    if (opt == "#SRC_TEXT")
    {
        __src_supported.at(opt)(this);
        return 7;
    }

    //if (tokensBuf.size() < 11)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/11)";


    // Find texture from map by gr
    int gr = stoine(tokensBuf[1]);
    std::string gr_key;
    switch (gr)
    {
    //case 100: gr_key = "STAGEFILE"; break;
    //case 101: gr_key = "BACKBMP"; break;
    //case 102: gr_key = "BANNER"; break;
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

	__src_supported.at(opt)(this);

    return 1;
}

ParseRet SkinLR2::SRC_IMAGE()
{
	if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

		//return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
	if (useVideo && videoBuf && videoBuf->haveVideo)
	{
        refineRect(d, { 0, 0, videoBuf->getW(), videoBuf->getH() }, srcLine);
		auto psv = std::make_shared<SpriteVideo>(d.w, d.h, videoBuf);
		_sprites.push_back(psv);
	}
    else
    {
        if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
        _sprites.push_back(std::make_shared<SpriteAnimated>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x));
    }

    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);
	
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NUMBER()
{
	if (tokensBuf.size() < 13)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_number d;
	convertLine(tokensBuf, (int*)&d, 0, 13);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

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
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_SLIDER()
{
	if (tokensBuf.size() < 13)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_slider d;
	convertLine(tokensBuf, (int*)&d, 0, 14); // 14th: mouse_disable is ignored for now
	
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

	_sprites.push_back(std::make_shared<SpriteSlider>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), (SliderDirection)d.muki, d.range, d.div_y*d.div_x, d.cycle, (eSlider)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);
	
    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BARGRAPH()
{
	if (tokensBuf.size() < 12)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/12)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_bargraph d;
	convertLine(tokensBuf, (int*)&d, 0, 12);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

	_sprites.push_back(std::make_shared<SpriteBargraph>(
		textureBuf, Rect(d.x, d.y, d.w, d.h), (BargraphDirection)d.muki, d.div_y*d.div_x, d.cycle, (eBargraph)d.type, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BUTTON()
{
	if (tokensBuf.size() < 13)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_button d;
	convertLine(tokensBuf, (int*)&d, 0, 13);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
	
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
				textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
			s->setInd(SpriteOption::opType::SWITCH, (unsigned)*sw);
			_sprites.push_back(s);
            _sprites_child.push_back(_sprites.back());
            _sprites.back()->setSrcLine(srcLine);
		}
		else if (auto op = std::get_if<eOption>(&buttonAdapter[d.type]))
		{
			auto s = std::make_shared<SpriteOption>(
				textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
			s->setInd(SpriteOption::opType::OPTION, (unsigned)*op);
			_sprites.push_back(s);
            _sprites_child.push_back(_sprites.back());
            _sprites.back()->setSrcLine(srcLine);
		}
	}
    else if (d.type == 200)
    {
        // SRC type 200 (¥á¥¤¥ó¥Ø¥ë¥×Æð„Ó)
        // why

        // FIXME don't know what to do on this case, deal as false

        auto s = std::make_shared<SpriteOption>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
        s->setInd(SpriteOption::opType::SWITCH, (unsigned)eSwitch::_FALSE);
        _sprites.push_back(s);
        _sprites_child.push_back(_sprites.back());
        _sprites.back()->setSrcLine(srcLine);
    }
    else
    {
        // deal as eSwitch::_FALSE
        auto s = std::make_shared<SpriteOption>(
            textureBuf, Rect(d.x, d.y, d.w, d.h), 1, 0, eTimer::SCENE_START, d.div_y, d.div_x, false);
        s->setInd(SpriteOption::opType::SWITCH, (unsigned)eSwitch::_FALSE);
        _sprites.push_back(s);
        _sprites_child.push_back(_sprites.back());
        _sprites.back()->setSrcLine(srcLine);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_ONMOUSE()
{
    if (tokensBuf.size() < 15)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
    }

    lr2skin::s_onmouse d;
    convertLine(tokensBuf, (int*)&d, 0, 15);

    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    _sprites.push_back(std::make_shared<SpriteOnMouse>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, d.panel, Rect(d.x2, d.y2, d.w2, d.h2), (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_MOUSECURSOR()
{
    if (tokensBuf.size() < 10)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
    }

    lr2skin::s_mousecursor d;
    convertLine(tokensBuf, (int*)&d, 0, 10);

    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    _sprites.push_back(std::make_shared<SpriteCursor>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_TEXT()
{
	if (tokensBuf.size() < 4)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/4)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_text d;
	convertLine(tokensBuf, (int*)&d, 0, 4);

	_sprites.push_back(std::make_shared<SpriteText>(
		_fontNameMap[std::to_string(d.font)], (eText)d.st, (TextAlign)d.align));
    _sprites_child.push_back(_sprites.back());
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

#pragma region SRC: Play skin specified

ParseRet SkinLR2::SRC_JUDGELINE()
{
    if (tokensBuf.size() < 10)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
    }

    lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)& d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    gSprites[GLOBAL_SPRITE_IDX_JUDGELINE] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[GLOBAL_SPRITE_IDX_JUDGELINE]->setSrcLine(srcLine);

    auto p = std::make_shared<SpriteGlobal>(GLOBAL_SPRITE_IDX_JUDGELINE);
    _sprites.push_back(p);
    _sprites_child.push_back(p);
    _sprites.back()->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWJUDGE(size_t idx)
{
    if (tokensBuf.size() < 10)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
    }

    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Nowjudge idx out of range (Line " << srcLine << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)& d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    gSprites[idx] = std::make_shared<SpriteAnimated>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer, d.div_y, d.div_x);
    gSprites[idx]->setSrcLine(srcLine);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_NOWCOMBO(size_t idx)
{
    if (tokensBuf.size() < 13)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
    }

    if (idx >= SPRITE_GLOBAL_MAX)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Nowjudge idx out of range (Line " << srcLine << ")";
        return ParseRet::PARAM_INVALID;
    }

    lr2skin::s_number d;
    convertLine(tokensBuf, (int*)& d, 0, 13);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

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
    gSprites[idx]->setSrcLine(srcLine);
    std::reinterpret_pointer_cast<SpriteNumber>(gSprites[idx])->setInhibitZero(true);

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_GROOVEGAUGE()
{
	if (tokensBuf.size() < 12)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/12)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_groovegauge d;
	convertLine(tokensBuf, (int*)&d, 0, 12);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

	if (d.div_y * d.div_x < 4)
	{
		LOG_WARNING << "[Skin] " << srcLine << ": div not enough (Line " << srcLine << ")";
        return ParseRet::DIV_NOT_ENOUGH;
	}

    size_t idx = d._null == 0 ? GLOBAL_SPRITE_IDX_1PGAUGE : GLOBAL_SPRITE_IDX_2PGAUGE;
    eNumber en = d._null == 0 ? eNumber::PLAY_1P_GROOVEGAUGE : eNumber::PLAY_2P_GROOVEGAUGE;

    gSprites[idx] = std::make_shared<SpriteGaugeGrid>(
        textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x / 4, d.cycle, d.add_x, d.add_y, 0, 100, 50,
        (eTimer)d.timer, en, d.div_y, d.div_x);
    gSprites[idx]->setSrcLine(srcLine);

    auto p = std::make_shared<SpriteGlobal>(idx);
    _sprites.push_back(p);
    _sprites_child.push_back(p);
    _sprites.back()->setSrcLine(srcLine);
	
    return ParseRet::OK;
}


ParseRet SkinLR2::SRC_NOWJUDGE1()
{
	if (tokensBuf.size() < 11)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/11)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

    lr2skin::s_nowjudge d;
    convertLine(tokensBuf, (int*)&d, 0, 11);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

	bufJudge1PSlot = d._null;
    if (bufJudge1PSlot >= 0 && bufJudge1PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_1PJUDGE + bufJudge1PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            noshiftJudge1P[bufJudge1PSlot] = stoine(tokensBuf[10]);
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setSrcLine(srcLine);
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
	if (tokensBuf.size() < 11)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/11)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

    lr2skin::s_nowjudge d;
    convertLine(tokensBuf, (int*)&d, 0, 11);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    bufJudge2PSlot = d._null;
    if (bufJudge2PSlot >= 0 && bufJudge2PSlot < 6)
    {
        size_t idx = GLOBAL_SPRITE_IDX_2PJUDGE + bufJudge2PSlot;
        auto ret = SRC_NOWJUDGE(idx);
        if (ret == ParseRet::OK)
        {
            noshiftJudge2P[bufJudge2PSlot] = stoine(tokensBuf[10]);
            auto p = std::make_shared<SpriteGlobal>(idx);
            _sprites.push_back(p);
            _sprites_child.push_back(p);
            _sprites.back()->setSrcLine(srcLine);
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
	if (tokensBuf.size() < 13)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

    lr2skin::s_nowcombo d;
    convertLine(tokensBuf, (int*)&d, 0, 11);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    bufJudge1PSlot = d._null;
	tokensBuf[10] = std::to_string((int)eNumber::_DISP_NOWCOMBO_1P);
    switch (stoine(tokensBuf[11]))
    {
    case 0: tokensBuf[11] = "1"; break;
    case 1: tokensBuf[11] = "2"; break;
    case 2:
    default:tokensBuf[11] = "0"; break;
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
            _sprites.back()->setSrcLine(srcLine);
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
	if (tokensBuf.size() < 13)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/12)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

    lr2skin::s_nowcombo d;
    convertLine(tokensBuf, (int*)&d, 0, 11);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    bufJudge2PSlot = d._null;
	tokensBuf[10] = std::to_string((int)eNumber::_DISP_NOWCOMBO_2P);
    switch (stoine(tokensBuf[11]))
    {
    case 0: tokensBuf[11] = "1"; break;
    case 1: tokensBuf[11] = "2"; break;
    case 2: 
    default:tokensBuf[11] = "0"; break;
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
            _sprites.back()->setSrcLine(srcLine);
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
    Sc1,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    _,
    _,

    Sc2,
    K8,
    K9,
    K10,
    K11,
    K12,
    K13,
    K14,
    _,
    _,
};

ParseRet SkinLR2::SRC_NOTE()
{
    // skip unsupported
    if (optBuf != "#SRC_LINE" &&
        !std::regex_match(optBuf, std::regex("#SRC_(AUTO_)?(NOTE|MINE|LN_END|LN_BODY|LN_START)")))
        return ParseRet::SRC_DEF_WRONG_TYPE;

    // load line into data struct
	lr2skin::s_basic d;
    convertLine(tokensBuf, (int*)&d);

    // TODO convert timer
    eTimer iTimer = (eTimer)d.timer;

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

    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    // SRC
    //if (tokensBuf.size() < 10)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

    if (d._null >= 20)
    {
        return ParseRet::PARAM_INVALID;
    }

    NoteLaneCategory cat = NoteLaneCategory::_;
    NoteLaneIndex idx = NoteLaneIndex::_;
    int ret = 0;

    // SRC_NOTE
    if (optBuf == "#SRC_NOTE")
    {
        cat = NoteLaneCategory::Note;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        ret = 1;
    }
    else if (optBuf == "#SRC_LINE")
    {
        cat = NoteLaneCategory::EXTRA;
        idx = (NoteLaneIndex)EXTRA_BARLINE;
        ret = 2;
    }
    else if (optBuf == "#SRC_LN_START")
    {
        cat = NoteLaneCategory::LN;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        ret = 3;
    }
    else if (optBuf == "#SRC_LN_BODY")
    {
        cat = NoteLaneCategory::LN;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        ret = 4;
    }
    else if (optBuf == "#SRC_LN_END")
    {
        cat = NoteLaneCategory::LN;
        idx = (NoteLaneIndex)NoteIdxToLaneMap[d._null];
        ret = 5;
    }
    else
    {
        // other types not supported
        LOG_WARNING << "[Skin] " << srcLine << ": \"" << optBuf << "\" is not supported yet";
        ret = 0;
        return ParseRet::OK;
    }

    size_t i = channelToIdx(cat, idx);
    if (i == CHANNEL_INVALID)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Note channel illegal: " << unsigned(cat) << ", " << unsigned(idx);
        return ParseRet::PARAM_INVALID;
    }

    switch (ret)
    {
    case 1:
    case 2:
        _sprites.push_back(std::make_shared<SpriteLaneVertical>(
            _textureNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, iTimer, d.div_y, d.div_x, false, !!(d._null >= 20)));
        _sprites_child.push_back(_sprites.back());

        _laneSprites[i] = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
        _laneSprites[i]->setLane(cat, idx);
        _laneSprites[i]->pNote->appendKeyFrame({ 0, {Rect(),
            RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0.0 } });
        _laneSprites[i]->pNote->setLoopTime(0);
        break;

    case 3:
    case 4:
    case 5:
    {
        if (_laneSprites[i] == nullptr)
        {
            _sprites.push_back(std::make_shared<SpriteLaneVerticalLN>(!!(d._null >= 20)));
            _sprites_child.push_back(_sprites.back());
            _laneSprites[i] = std::static_pointer_cast<SpriteLaneVerticalLN>(_sprites.back());
            _laneSprites[i]->setLane(cat, idx);
        }

        auto p = std::static_pointer_cast<SpriteLaneVerticalLN>(_laneSprites[i]);
        std::shared_ptr<SpriteAnimated> *pn = nullptr;
        switch (ret)
        {
        case 3:
            pn = &p->pNote;
            break;
        case 4:
            pn = &p->pNoteBody;
            break;
        case 5:
            pn = &p->pNoteTail;
            break;
        default:
            break;
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
	if (tokensBuf.size() < 12)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/12)";

		//return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_bga d;
	convertLine(tokensBuf, (int*)&d);

	_sprites.push_back(std::make_shared<SpriteStatic>(gPlayContext.bgaTexture, Rect()));

	_sprites_child.push_back(_sprites.back());
	_sprites.back()->setSrcLine(srcLine);

	return ParseRet::OK;
}

#pragma endregion

#pragma region SRC: Select skin specified

ParseRet SkinLR2::SRC_BAR_BODY()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    size_t type = d._null & 0xFFFFFFFF;

    for (auto& bar : _barSprites)
    {
        bar->setBody(BarType(type),
            textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_FLASH()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);

    for (auto& bar : _barSprites)
    {
        bar->setFlash(textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteFlash()->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_LEVEL()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_number d;
	convertLine(tokensBuf, (int*)&d, 0, 13);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
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
        bar->getSpriteLevel(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_LAMP()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setLamp(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteLamp(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_TITLE()
{
    if (tokensBuf.size() < 4)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/4)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_text d;
	convertLine(tokensBuf, (int*)&d, 0, 4);
    BarTitleType type = BarTitleType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setTitle(type, _fontNameMap[std::to_string(d.font)], (TextAlign)d.align);
        bar->getSpriteTitle(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RANK()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    BarRankType type = BarRankType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRank(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteRank(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    BarRivalType type = BarRivalType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalWinLose(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteRivalWinLose(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_MYLAMP()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampSelf(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteRivalLampSelf(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::SRC_BAR_RIVAL_RIVALLAMP()
{
    if (tokensBuf.size() < 10)
	{
		//LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/10)";

        //return ParseRet::PARAM_NOT_ENOUGH;
	}

	lr2skin::s_basic d;
	convertLine(tokensBuf, (int*)&d);
    if (textureBuf) refineRect(d, textureBuf->getRect(), srcLine);
    BarLampType type = BarLampType(d._null & 0xFFFFFFFF);

    for (auto& bar : _barSprites)
    {
        bar->setRivalLampRival(type, textureBuf, Rect(d.x, d.y, d.w, d.h), d.div_y * d.div_x, d.cycle, (eTimer)d.timer);
        bar->getSpriteRivalLampRival(type)->setParent(bar->weak_from_this());
    }

    return ParseRet::OK;
}


#pragma endregion

int SkinLR2::DST()
{
    auto opt = optBuf;

    static const std::map<Token, int> __general_dst_supported
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
        {"#DST_BGA",14},
        {"#DST_MOUSECURSOR",15},
    };

    if (__general_dst_supported.find(opt) == __general_dst_supported.end() || _sprites.empty())
        return 0;

    if (tokensBuf.size() < 15)
    {
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/15)";

    }

    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 15);
    

    int ret = 0;
    auto e = _sprites.back();
    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Previous src definition invalid (Line: " << srcLine << ")";
        return 0;
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
                LOG_WARNING << "[Skin] " << srcLine << ": Previous src definition invalid (Line: " << srcLine << ")";
                return 0;
            }

        } while (enext->type() == SpriteTypes::GLOBAL);
    }

	ret = __general_dst_supported.at(opt);

    // DST_TEXT align adjust
    if (e->type() == SpriteTypes::TEXT)
    {
        auto p = std::reinterpret_pointer_cast<SpriteText>(e);
        switch (p->getAlignType())
        {
        case TEXT_ALIGN_CENTER:
            d.x -= d.w / 2;
            break;
        case TEXT_ALIGN_RIGHT:
            d.x -= d.w;
            break;
        default:
            break;
        }
    }

    if (e->isKeyFrameEmpty())
    {
        if (convertLine(tokensBuf, (int*)&d, 15, 2) >= 2)
			convertOps(tokensBuf, (int*)d.op, 17, 4);
        //else 
            //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/17)";


        if (std::regex_match(opt, std::regex("#DST_NOW(JUDGE|COMBO)_1P")))
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
        else if (std::regex_match(opt, std::regex("#DST_NOW(JUDGE|COMBO)_2P")))
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
            //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
            e->appendInvisibleLeadingFrame(d.x, d.y);
        }
    }

    e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
		(BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << line << ": Set sprite Keyframe (time: " << d.time << ")";

    return ret;
}


ParseRet SkinLR2::DST_NOTE()
{
    if (optBuf != "#DST_NOTE")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);

    if (d._null >= 20)
        return ParseRet::PARAM_INVALID;

    if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
        convertOps(tokensBuf, (int*)d.op, 16, 4);
    //else
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";

	NoteLaneIndex idx = NoteLaneIndex(NoteIdxToLaneMap[d._null]);

    auto setDstNoteLambda = [&](NoteLaneCategory i, std::shared_ptr<SpriteLaneVertical> e)
    {
        /*
        e->pNote->clearKeyFrames();
        e->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center)  } });
            */

        drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
        e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
        e->setLoopTime(0);
        //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
        //LOG_DEBUG << "[Skin] " << line << ": Set Lane sprite Keyframe (time: " << d.time << ")";
    };

    auto e1 = _laneSprites[channelToIdx(NoteLaneCategory::Note, idx)];
    setDstNoteLambda(NoteLaneCategory::Note, e1);

    // TODO add mine sprite
    //setDstNoteLambda(NoteLaneCategory::Mine);

    auto e3 = _laneSprites[channelToIdx(NoteLaneCategory::LN, idx)];
    setDstNoteLambda(NoteLaneCategory::LN, e3);

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_LINE()
{
    if (optBuf != "#DST_LINE")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    

    int ret = 0;
    auto e = _sprites.back();
    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Barline SRC definition invalid " <<
            "(Line: " << srcLine << ")";
        return ParseRet::SRC_DEF_INVALID;
    }

    if (e->type() != SpriteTypes::NOTE_VERT)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Barline SRC definition is not NOTE " <<
            "(Line: " << srcLine << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }
    if (!e->isKeyFrameEmpty())
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Barline DST is already defined " <<
            "(Line: " << srcLine << ")";
        e->clearKeyFrames();
    }

    // set sprite channel
    auto p = std::static_pointer_cast<SpriteLaneVertical>(e);

    p->playerSlot = d._null / 10;  // player slot, 1P:0, 2P:1

    NoteLaneCategory cat = p->getLaneCat();
    NoteLaneIndex idx = p->getLaneIdx();
    if (cat != NoteLaneCategory::EXTRA || idx != EXTRA_BARLINE)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Previous SRC definition is not LINE " <<
            "(Line: " << srcLine << ")";
        return ParseRet::SRC_DEF_WRONG_TYPE;
    }

    p->pNote->clearKeyFrames();
    p->pNote->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

    if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
        convertOps(tokensBuf, (int*)d.op, 16, 4);
    //else 
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


    drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
    e->appendKeyFrame({ 0, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
        (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });
    e->setLoopTime(0);
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    //LOG_DEBUG << "[Skin] " << line << ": Set Lane sprite (Barline) Keyframe (time: " << d.time << ")";

    return ParseRet::OK;
}


ParseRet SkinLR2::DST_BAR_BODY()
{
    if (optBuf != "#DST_BAR_BODY_OFF" && optBuf != "#DST_BAR_BODY_ON")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    bool bodyOn = optBuf == "#DST_BAR_BODY_ON";

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    
    unsigned idx = unsigned(d._null);

    for (BarType type = BarType(0); type != BarType::TYPE_COUNT; ++*(unsigned*)&type)
    {
        auto e = bodyOn ? _barSprites[idx]->getSpriteBodyOn(type) : _barSprites[idx]->getSpriteBodyOff(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_BODY undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }

            if (!_barSpriteAdded[idx])
            {
                _barSprites[idx]->setSrcLine(srcLine);
                _barSpriteAdded[idx] = true;
                drawQueue.push_back({ _barSprites[idx], false, d.op[0], d.op[1], d.op[2], d.op[3] });
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
    if (optBuf != "#DST_BAR_FLASH")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteFlash();
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_FLASH undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::FLASH);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LEVEL()
{
    if (optBuf != "#DST_BAR_LEVEL")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    
    BarLevelType type = BarLevelType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteLevel(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_LEVEL " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LEVEL);
    }


    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL_MYLAMP()
{
    if (optBuf != "#DST_BAR_MY_LAMP")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalLampSelf(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_MY_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::MYLAMP);
    }

    return ParseRet::OK;
}
ParseRet SkinLR2::DST_BAR_RIVAL_RIVALLAMP()
{
    if (optBuf != "#DST_BAR_RIVAL_LAMP")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalLampRival(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_RIVAL_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RIVALLAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_LAMP()
{
    if (optBuf != "#DST_BAR_LAMP")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);
    
    auto type = BarLampType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteLamp(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_LAMP " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::LAMP);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_TITLE()
{
    if (optBuf != "#DST_BAR_TITLE")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);

    auto type = BarTitleType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteTitle(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_TITLE undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::TITLE);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RANK()
{
    if (optBuf != "#DST_BAR_RANK")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);

    auto type = BarRankType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRank(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_RANK " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
        }

        e->appendKeyFrame({ d.time, {Rect(d.x, d.y, d.w, d.h), (RenderParams::accTy)d.acc, Color(d.r, d.g, d.b, d.a),
            (BlendMode)d.blend, !!d.filter, (double)d.angle, getCenterPoint(d.w, d.h, d.center) } });

        bar->pushPartsOrder(BarPartsType::RANK);
    }

    return ParseRet::OK;
}

ParseRet SkinLR2::DST_BAR_RIVAL()
{
    if (optBuf != "#DST_BAR_RIVAL")
        return ParseRet::SRC_DEF_WRONG_TYPE;

    //if (tokensBuf.size() < 13)
        //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/13)";


    // load line into data struct
    lr2skin::dst d;
    convertLine(tokensBuf, (int*)&d, 0, 14);

    auto type = BarRivalType(d._null);

    for (auto& bar : _barSprites)
    {
        auto e = bar->getSpriteRivalWinLose(type);
        if (e == nullptr)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": SRC_BAR_RIVAL " << std::to_string(size_t(type)) << " undefined";
            return ParseRet::SRC_DEF_INVALID;
        }

        if (e->isKeyFrameEmpty())
        {
            if (convertLine(tokensBuf, (int*)&d, 14, 2) >= 2)
                convertOps(tokensBuf, (int*)d.op, 16, 4);
            //else
                //LOG_WARNING << "[Skin] " << line << ": Parameter not enough (" << tokensBuf.size() << "/16)";


            e->setSrcLine(srcLine);
            e->setLoopTime(d.loop);
            e->setTrigTimer((eTimer)d.timer);
            if (d.time > 0)
            {
                //LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
                e->appendInvisibleLeadingFrame(d.x, d.y);
            }
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
#pragma region

int SkinLR2::parseBody(const Tokens &raw)
{
    if (raw.empty()) return 0;
    tokensBuf.assign(raw.size() - 1, "");
    optBuf = raw[0];
    for (size_t idx = 0; idx < tokensBuf.size(); ++idx)
        tokensBuf[idx] = raw[idx + 1];

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
        if (DST_BAR_BODY() == ParseRet::OK)
            return 12;
        if (DST_BAR_FLASH() == ParseRet::OK)
            return 13;
        if (DST_BAR_LEVEL() == ParseRet::OK)
            return 14;
        if (DST_BAR_LAMP() == ParseRet::OK)
            return 15;
        if (DST_BAR_TITLE() == ParseRet::OK)
            return 16;
        if (DST_BAR_RANK() == ParseRet::OK)
            return 17;
        if (DST_BAR_RIVAL() == ParseRet::OK)
            return 18;
        if (DST_BAR_RIVAL_MYLAMP() == ParseRet::OK)
            return 19;
        if (DST_BAR_RIVAL_RIVALLAMP() == ParseRet::OK)
            return 20;

        LOG_WARNING << "[Skin] " << srcLine << ": Invalid def \"" << optBuf << "\" (Line " << srcLine << ")";
    }
    catch (std::invalid_argument e)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Invalid Argument: " << "(Line " << srcLine << ")";
    }
    catch (std::out_of_range e)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": Out of range: " << "(Line " << srcLine << ")";
    }
    return 0;
}

void SkinLR2::IF(const Tokens &t, std::ifstream& lr2skin)
{
    bool ifCheckPassed = true;
    if (t[0] != "#ELSE" && t.size() <= 1)
    {
        LOG_WARNING << "[Skin] " << srcLine << ": No IF parameters " << " (Line " << srcLine << ")";
    }

    // get dst indexes
    for (auto it = ++t.begin(); it != t.end() && ifCheckPassed; ++it)
    {
        auto [idx, val] = stoub(*it);
        if (idx == -1)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": Invalid DST_OPTION Index, deal as false (Line " << srcLine << ")";
            ifCheckPassed = false;
            break;
        }
        bool dst = getDstOpt((dst_option)idx);
        if (val) dst = !dst;
        ifCheckPassed = ifCheckPassed && dst;
    }

    if (ifCheckPassed)
    {
        while (!lr2skin.eof())
        {
            auto tokens = csvNextLineTokenize(lr2skin);
            if (tokens.empty()) continue;

            if (*tokens.begin() == "#ELSE" || *tokens.begin() == "#ELSEIF")
            {
                // skip other branches
                while (!lr2skin.eof() && !tokens.empty() && *tokens.begin() != "#ENDIF")
                {
                    tokens = csvNextLineTokenize(lr2skin);
                }
                return;
            }
            else if (*tokens.begin() == "#ENDIF")
            {
                // end #IF process
                return;
            }
            else
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

int SkinLR2::parseHeader(const Tokens& raw)
{
    if (raw.empty()) return 0;
    tokensBuf.assign(raw.size() - 1, "");
    optBuf = raw[0];
    for (size_t idx = 0; idx < tokensBuf.size(); ++idx)
        tokensBuf[idx] = raw[idx + 1];

    if (optBuf == "#INFORMATION")
    {
        while (tokensBuf.size() < 4) tokensBuf.push_back("");

        int type = stoine(tokensBuf[0]);
        StringContent title = tokensBuf[1];
        StringContent maker = tokensBuf[2];
        Path thumbnail(tokensBuf[3]);

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

        _textureNameMap["THUMBNAIL"] = std::make_shared<Texture>(Image(thumbnail.string().c_str()));
        if (_textureNameMap["THUMBNAIL"] == nullptr)
            LOG_WARNING << "[Skin] " << srcLine << ": thumbnail loading failed: " << thumbnail.string() << " (Line " << srcLine << ")";

        LOG_DEBUG << "[Skin] " << srcLine << ": Loaded metadata: " << title << " | " << maker;

        return 1;
    }

    else if (optBuf == "#CUSTOMOPTION")
    {
        StringContent title = tokensBuf[0];
        int dst_op = stoine(tokensBuf[1]);
        if (dst_op < 900 || dst_op > 999)
        {
            LOG_WARNING << "[Skin] " << srcLine << ": Invalid option value: " << dst_op << " (Line " << srcLine << ")";
            return -2;
        }
        Tokens op_label;
        for (size_t idx = 3; idx < tokensBuf.size() && !tokensBuf[idx].empty(); ++idx)
            op_label.push_back(tokensBuf[idx]);

        LOG_DEBUG << "[Skin] " << srcLine << ": Loaded Custom option " << title << ": " << dst_op;
        customize.push_back({ (unsigned)dst_op, title, std::move(op_label), 0 });
        return 2;
    }

    else if (optBuf == "#CUSTOMFILE")
    {
        StringContent title = tokensBuf[0];
        StringContent p = tokensBuf[1];
        Path pathf(p);
        Path def(tokensBuf[2]);

        auto ls = findFiles(pathf);
        size_t defVal = 0;
        for (size_t param = 0; param < ls.size(); ++param)
            if (ls[param].filename().stem() == def)
            {
                defVal = param;
                break;
            }

        LOG_DEBUG << "[Skin] " << srcLine << ": Loaded Custom file " << title << ": " << pathf.string();
        customFile.push_back({ title, p, std::move(ls), defVal, defVal });
        return 3;
    }

    else if (optBuf == "#ENDOFHEADER")
    {
        return -1;
    }

    return 0;
}

#pragma endregion

SkinLR2::SkinLR2(Path p)
{
    for (size_t i = 0; i < BAR_ENTRY_SPRITE_COUNT; ++i)
    {
        _barSprites[i] = std::make_shared<SpriteBarEntry>(i);
        _sprites.push_back(_barSprites[i]);
        _sprites_parent.push_back(_barSprites[i]);
    }
	_laneSprites.resize(CHANNEL_COUNT);
    updateDstOpt();
    loadCSV(p);

    for (auto& p : _sprites)
    {
        if (p->type() == SpriteTypes::VIDEO)
        {
            auto v = std::reinterpret_pointer_cast<SpriteVideo>(p);
            v->startPlaying();
        }
    }
}

void SkinLR2::loadCSV(Path p)
{
    srcLine = 0;
    LOG_DEBUG << "[Skin] File: " << p.string();
    std::ifstream lr2skin(p, std::ios::binary);
    if (!lr2skin.is_open())
    {
        LOG_ERROR << "[Skin] File Not Found: " << std::filesystem::absolute(p).string();
        return;
    }

    bool haveEndOfHeader = false;
    while (!lr2skin.eof())
    {
        auto raw = csvNextLineTokenize(lr2skin);
        if (raw.begin() == raw.end()) continue;

        if (parseHeader(raw) == -1)
        {
            haveEndOfHeader = true;
            break;
        }
    }
    LOG_DEBUG << "[Skin] File: " << p.string() << "(Line " << srcLine << "): Header loading finished";

    if (lr2skin.eof())
    {
        // reset position to head
        lr2skin.close();
        srcLine = 0;
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


    LOG_DEBUG << "[Skin] File: " << p.string() << "(Line " << srcLine << "): Body loading finished";
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

    int ttAngle1P = gNumbers.get(eNumber::_ANGLE_TT_1P);
    int ttAngle2P = gNumbers.get(eNumber::_ANGLE_TT_2P);

    {
        // acquire lock
        std::lock_guard<std::mutex> u(gSelectContext._mutex);

#ifndef _DEBUG
        std::for_each(std::execution::par_unseq, drawQueue.begin(), drawQueue.end(), [ttAngle1P, ttAngle2P](auto& e)
#else
        for (auto& e : drawQueue)
#endif
        {
            e.draw = getDstOpt(e.op1) && getDstOpt(e.op2) && getDstOpt(e.op3);
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