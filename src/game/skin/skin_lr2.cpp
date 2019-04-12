#include "skin_lr2.h"
#include <plog/Log.h>
#include "utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <variant>
#include "game/data/option.h"
#include "game/data/switch.h"

#ifdef _WIN32
// For GetWindowsDirectory
#include <Windows.h>
#endif

bool SkinLR2::customizeDst[100];

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
	eOption::PLAY_GAUGE_TYPE,
	eSwitch::_FALSE,	// 2p
	eOption::PLAY_RANDOM_TYPE,
	eSwitch::_FALSE,	// 2p
	eSwitch::PLAY_OPTION_AUTOSCR,
	eSwitch::_FALSE,	// 2p
	
	//46~49
	eSwitch::_FALSE,	// shutter?
	eSwitch::_FALSE,
	eSwitch::_FALSE,	// reserved
	eSwitch::_FALSE,	// reserved

	//50~51
	eOption::PLAY_LANE_EFFECT_TYPE,
	eSwitch::_FALSE,	// 2p

	eSwitch::_FALSE,	// reserved
	eSwitch::_FALSE,	// reserved

	//54
	eSwitch::PLAY_OPTION_DP_FLIP,
	eOption::PLAY_HSFIX_TYPE,
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
	eOption::PLAY_GHOST_TYPE,
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

#pragma region LR2 csv parsing

Tokens SkinLR2::csvNextLineTokenize(std::istream& file)
{
    ++line;
    StringContent linecsv;
    std::getline(file, linecsv);

    if (linecsv.empty() || linecsv.substr(0, 2) == "//")
        return {};

    while (linecsv.length() > 0 && linecsv[linecsv.length() - 1] == '\r')
        linecsv.pop_back();

    if (linecsv.empty())
        return {};

    // replace "\" with "\\"
    //std::istringstream iss(linecsv);
    //StringContent buf, line;
    //while (std::getline(iss, buf, '\\'))
    //    line += buf + R"(\\)";
    //line.erase(line.length() - 2);
    //auto line = std::regex_replace(linecsv, std::regex(R"(\\)"), R"(\\\\)");

    //Tokens ret;
    //auto tokens = tokenizer(line, boost::escaped_list_separator<char>());
    //for (auto& t : tokens) ret.push_back(t);
    //while (!ret.empty() && ret.back().empty()) ret.pop_back();
    //return ret;
    const std::regex re{ R"(((?:[^\\,]|\\.)*?)(?:,|$))" };
    Tokens result = { std::sregex_token_iterator(linecsv.begin(), linecsv.end(), re, 1), std::sregex_token_iterator() };
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

void refineRect(lr2skin::s_basic& d, unsigned line)
{
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

int SkinLR2::loadLR2image(const Tokens &t)
{
    if (t[0] == "#IMAGE")
    {
        StringContent p = t[1];
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

int SkinLR2::loadLR2font(const Tokens &t)
{
	// TODO load LR2FONT
    if (t[0] == "#LR2FONT")
    {
        Path path(t[1]);
        //lr2fontPath.push_back(std::move(path));
        LOG_DEBUG << "[Skin] " << line << ": Skipped LR2FONT: " << path.string();
        return 1;
    }
    return 0;
}

int SkinLR2::loadLR2systemfont(const Tokens &t)
{
	// Could not get system font file path in a reliable way while cross-platforming..
    if (t[0] == "#FONT")
    {
        int ptsize = stoine(t[1]);
        int thick = stoine(t[2]);
        int fonttype = stoine(t[3]);
        //StringContent name = t[4];
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

int SkinLR2::loadLR2include(const Tokens &t)
{
    if (t[0] == "#INCLUDE")
    {
        Path path(t[1]);
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

int SkinLR2::loadLR2timeoption(const Tokens &t)
{
    if (t[0] == "#STARTINPUT")
    {
        info.timeIntro = stoine(t[1]);
        if (info.mode == eMode::RESULT || info.mode == eMode::COURSE_RESULT)
        {
            int rank = stoine(t[2]);
            int update = stoine(t[3]);
            //if (rank > 0) info.resultStartInputTimeRank = rank;
            //if (update > 0) info.resultStartInputTimeUpdate = update;
            LOG_DEBUG << "[Skin] " << line << ": Skipped STARTINPUT " << rank << " " << update;
        }

        return 1;
    }

    else if (t[0] == "#SKIP")
    {
        int time = stoine(t[1]);
        info.timeIntro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set Intro freeze time: " << time;
        return 2;
    }

    else if (t[0] == "#LOADSTART")
    {
        int time = stoine(t[1]);
        info.timeStartLoading = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown before loading: " << time;
        return 3;
    }

    else if (t[0] == "#LOADEND")
    {
        int time = stoine(t[1]);
        info.timeMinimumLoad = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown after loading: " << time;
        return 4;
    }

    else if (t[0] == "#PLAYSTART")
    {
        int time = stoine(t[1]);
        info.timeGetReady = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time READY after loading: " << time;
        return 5;
    }

    else if (t[0] == "#CLOSE")
    {
        int time = stoine(t[1]);
        info.timeFailed = time;
        LOG_DEBUG << "[Skin] " << line << ": Set FAILED time length: " << time;
        return 6;
    }

    else if (t[0] == "#FADEOUT")
    {
        int time = stoine(t[1]);
        info.timeOutro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set fadeout time length: " << time;
        return 7;
    }

    return 0;
}

int SkinLR2::loadLR2others(const Tokens &t)
{
    if (t[0] == "#RELOADBANNER")
    {
        reloadBanner = true;
        LOG_DEBUG << "[Skin] " << line << ": Set dynamic banner loading";
        return 1;
    }
    if (t[0] == "#TRANSCOLOR")
    {
        int r, g, b;
        r = stoine(t[1]);
        g = stoine(t[2]);
        b = stoine(t[3]);
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
    if (t[0] == "#FLIPSIDE")
    {
        flipSide = true;
        return 3;
    }
    if (t[0] == "#FLIPRESULT")
    {
        flipResult = true;
        return 4;
    }
    if (t[0] == "#DISABLEFLIP")
    {
        disableFlipResult = true;
        return 5;
    }
    if (t[0] == "#SCRATCH")
    {
        int a, b;
        a = !!stoine(t[1]);
        b = !!stoine(t[2]);
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

int SkinLR2::loadLR2src(const Tokens &t)
{
    auto opt = t[0];

    if (opt == "#SRC_TEXT")
    {
        if (t.size() < 5)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        lr2skin::s_text src{ 0 };
        for (size_t i = 0; i < sizeof(src) / sizeof(int); ++i)
            *((int*)&src + i) = stoine(t[i + 1]);

		_sprites.push_back(std::make_shared<SpriteText>(
			_fontNameMap[std::to_string(src.font)], (eText)src.st, (TextAlign)src.align));

        LOG_DEBUG << "[Skin] " << line << ": Added Text (font: " << src.font << ")";

        return 7;
    }

    // skip unsupported
    if (opt != "#SRC_IMAGE" && opt != "#SRC_NUMBER" &&
        opt != "#SRC_SLIDER" && opt != "#SRC_BARGRAPH" &&
        opt != "#SRC_BUTTON" && opt != "#SRC_ONMOUSE")
        return 0;

    if (t.size() < 11)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // load line into data struct
    int src[32]{ 0 };
    convertLine(t, src);
    lr2skin::s_basic& d = *(lr2skin::s_basic*)src;
    refineRect(d, line);

    // TODO convert timer
    eTimer iTimer = (eTimer)d.timer;

    // Find texture from map by gr
    pTexture tex = nullptr;
    std::string gr_key = std::to_string(d.gr);
    if (_texNameMap.find(gr_key) != _texNameMap.end())
    {
        tex = _texNameMap[gr_key];
        if (d.w == -1)
            d.w = tex->getRect().w;
        if (d.h == -1)
            d.h = tex->getRect().h;
    }
    else
    {
        tex = _texNameMap["Error"];
        d.x = d.y = 0;
        d.w = d.h = 1;
    }

    int ret = 0;

    if (opt == "#SRC_IMAGE")
    {
        _sprites.push_back(std::make_shared<SpriteAnimated>(
            _texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y, d.div_x, d.cycle, iTimer));
        LOG_DEBUG << "[Skin] " << line << ": Set Image sprite (texture: " << gr_key << ", timer: " << d.timer << ")";
        ret = 1;
    }

    else if (opt == "#SRC_NUMBER")
    {
        if (t.size() < 14)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        convertLine(t, src, 10, 13);
        lr2skin::s_number& d = *(lr2skin::s_number*)src;

        // TODO convert num
        eNumber iNum = (eNumber)d.num;

		_sprites.emplace_back(std::make_shared<SpriteNumber>(
			_texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), (NumberAlign)d.align, d.keta, d.div_y, d.div_x, d.cycle, iNum, iTimer));
		LOG_DEBUG << "[Skin] " << line << ": Set Number sprite (gr: " << gr_key << ", num: " << (unsigned)iNum << ")";

        ret = 2;
    }
    else if (opt == "#SRC_SLIDER")
    {
        if (t.size() < 14)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        convertLine(t, src, 10, 13);
        lr2skin::s_slider& d = *(lr2skin::s_slider*)src;
		// 14th: mouse_disable is ignored for now

		_sprites.push_back(std::make_shared<SpriteSlider>(
            _texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), (SliderDirection)d.muki, d.range, d.div_y, d.div_x, d.cycle, (eSlider)d.type, iTimer));
        LOG_DEBUG << "[Skin] " << line << ": Set Slider sprite (texture: " << gr_key << ", type: " << d.type << ", timer: " << d.timer << ")";
        ret = 3;
    }
    else if (opt == "#SRC_BARGRAPH")
    {
        if (t.size() < 14)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        convertLine(t, src, 10, 13);
        lr2skin::s_bargraph& d = *(lr2skin::s_bargraph*)src;

		_sprites.push_back(std::make_shared<SpriteBargraph>(
            _texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), (BargraphDirection)d.muki, d.div_y, d.div_x, d.cycle, (eBargraph)d.type, iTimer));
        LOG_DEBUG << "[Skin] " << line << ": Set Bargraph sprite (texture: " << gr_key << ", type: " << d.type << ", timer: " << d.timer << ")";
        ret = 4;
    }
    else if (opt == "#SRC_BUTTON")
    {
        if (t.size() < 14)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        convertLine(t, src, 10, 13);
        lr2skin::s_button& d = *(lr2skin::s_button*)src;
		
		if (d.type < buttonAdapter.size())
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
					_texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), 1, 1, 0, eTimer::SCENE_START, false,
					d.div_y, d.div_x);
				s->setInd(SpriteOption::opType::SWITCH, (unsigned)*sw);
				_sprites.push_back(s);
			}
			else if (auto op = std::get_if<eOption>(&buttonAdapter[d.type]))
			{
				auto s = std::make_shared<SpriteOption>(
					_texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), 1, 1, 0, eTimer::SCENE_START, false,
					d.div_y, d.div_x);
				s->setInd(SpriteOption::opType::OPTION, (unsigned)*op);
				_sprites.push_back(s);
			}
		}

        LOG_DEBUG << "[Skin] " << line << ": Set Option sprite (texture: " << gr_key << ", timer: " << d.timer << ")";
        ret = 5;
    }
	/*
    else if (opt == "#SRC_ONMOUSE")
    {

        ret = 6;
    }
    */
    return ret;
}

int SkinLR2::loadLR2dst(const Tokens &t)
{
    auto opt = t[0];

    if (opt != "#DST_IMAGE" && opt != "#DST_NUMBER" && opt != "#DST_SLIDER" &&
        opt != "#DST_BARGRAPH" && opt != "#DST_BUTTON" && opt != "#DST_ONMOUSE" &&
        opt != "#DST_TEXT" || _sprites.empty())
        return 0;

    if (t.size() < 14)
    {
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
    }

    // load line into data struct
    int src[32]{ 0 };
    convertLine(t, src, 0, 14);
    lr2skin::dst& d = *(lr2skin::dst*)src;

    int ret = 0;
    auto e = _sprites.back();
    if (e == nullptr)
    {
        LOG_WARNING << "[Skin] " << line << ": Previous src definition invalid (Line: " << line << ")";
        return 0;
    }

    if (opt == "#DST_IMAGE")
    {
        ret = 1;
    }
    else if (opt == "#DST_NUMBER")
    {
        ret = 2;
    }
    else if (opt == "#DST_BARGRAPH")
    {
        ret = 3;
    }
    else if (opt == "#DST_BUTTON")
    {
        ret = 4;
    }
    else if (opt == "#DST_ONMOUSE")
    {
        ret = 5;
    }

    if (e->isKeyFrameEmpty())
    {
        convertLine(t, src, 14, 16);
        //lr2skin::dst& d = *(lr2skin::dst*)src;
        if (t.size() < 17 || t[16].empty())
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            d.loop = -1;
            d.timer = 0;
            d.op[0] = d.op[1] = d.op[2] = d.op[3] = DST_TRUE;
        }
        else
        {
			convertLine(t, src, 16, 17);
            for (size_t i = 0; i < 4; ++i)
            {
                StringContent ops = t[18 + i];
                if (ops[0] == '!' || ops[0] == '-')
                    *(int*)&d.op[i] = -stoine(ops.substr(1));
                else
                    *(int*)&d.op[i] = stoine(ops);
            }
        }

        drawQueue.push_back({ e, false, d.op[0], d.op[1], d.op[2], d.op[3] });
        e->setLoopTime(d.loop);
		e->setTimer((eTimer)d.timer);
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
    LOG_DEBUG << "[Skin] " << line << ": Set sprite Keyframe (time: " << d.time << ")";

    return ret;
}

int SkinLR2::loadLR2srcnote(const Tokens &t)
{
    // skip unsupported
    if ( !(t[0] == "#SRC_NOTE" || t[0] == "#SRC_MINE" || t[0] == "#SRC_LN_END" || t[0] == "#SRC_LN_BODY"
        || t[0] == "#SRC_LN_START"|| t[0] == "#SRC_AUTO_NOTE" || t[0] == "#SRC_AUTO_MINE" || t[0] == "#SRC_AUTO_LN_END" 
        || t[0] == "#SRC_AUTO_LN_BODY" || t[0] == "#SRC_AUTO_LN_START"))
    {
        return 0;
    }

    // load line into data struct
    int src[32]{ 0 };
    convertLine(t, src);
    lr2skin::s_basic& d = *(lr2skin::s_basic*)src;
    refineRect(d, line);

    // TODO convert timer
    eTimer iTimer = (eTimer)d.timer;

    // Find texture from map by gr
    pTexture tex = nullptr;
    std::string gr_key = std::to_string(d.gr);
    if (_texNameMap.find(gr_key) != _texNameMap.end())
    {
        tex = _texNameMap[gr_key];
        if (d.w == -1 && d.h == -1)
        {
            d.w = tex->getRect().w;
            d.h = tex->getRect().h;
        }
    }
    else
    {
        tex = _texNameMap["Error"];
        d.x = d.y = 0;
        d.w = d.h = 1;
    }

    // SRC
    if (t.size() < 11)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // SRC_NOTE
    if (t[0] == "#SRC_NOTE" || t[0] == "#SRC_AUTO_NOTE")
    {
		NoteChannelCategory cat = NoteChannelCategory::Note;
		NoteChannelIndex idx = (NoteChannelIndex)d._null;
		size_t i = channelToIdx(cat, idx);
        _sprites.push_back(std::make_shared<SpriteLaneVertical>(
            _texNameMap[gr_key], Rect(d.x, d.y, d.w, d.h), d.div_y, d.div_x, d.cycle, iTimer));
        _laneSprites[i] = std::static_pointer_cast<SpriteLaneVertical>(_sprites.back());
		_laneSprites[i]->setChannel(cat, idx);
        LOG_DEBUG << "[Skin] " << line << ": Set Note " << idx << " sprite (texture: " << gr_key << ", timer: " << d.timer << ")";

		_laneSprites[i]->pNote->appendKeyFrame({ 0, {Rect(),
			RenderParams::accTy::CONSTANT, Color(0xffffffff), BlendMode::ALPHA, 0, 0 } });
		_laneSprites[i]->pNote->setLoopTime(0);

        return 1;
    }
    
    // other types are not supported
    return 0;
}

int SkinLR2::loadLR2dstnote(const Tokens &t)
{
    if (t[0] != "#DST_NOTE")
        return 0;

    if (t.size() < 14)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    // load line into data struct
    int src[32]{ 0 };
    convertLine(t, src, 0, 14);
    lr2skin::dst& d = *(lr2skin::dst*)src;
	NoteChannelIndex idx = (NoteChannelIndex)d._null;

	for (size_t i = (size_t)NoteChannelCategory::Note; i < (size_t)NoteChannelCategory::_; ++i)
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

		// refine rect: x=dst_x, y=-dst_h, w=dst_w, h=dst_y
		int dummy, dst_h;
		//p->getRectSize(d.w, dummy);
		dst_h = d.h;
		d.h = d.y;
		d.y = -dst_h;

		convertLine(t, src, 14, 16);
		//lr2skin::dst& d = *(lr2skin::dst*)src;
		if (t.size() < 17 || t[16].empty())
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
				StringContent ops = t[18 + i];
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
		LOG_DEBUG << "[Skin] " << line << ": Set Lane sprite Keyframe (time: " << d.time << ")";
	}

    return 1;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Dispatcher
#pragma region

int SkinLR2::loadLR2SkinLine(const Tokens &raw)
{
    Tokens t;
    t.assign(30, "");
    for (size_t idx = 0; idx < raw.size(); ++idx)
        t[idx] = raw[idx];
    try {
        if (loadLR2image(t))
            return 1;
        if (loadLR2font(t))
            return 2;
        if (loadLR2systemfont(t))
            return 3;
        if (loadLR2include(t))
            return 4;
        if (loadLR2timeoption(t))
            return 5;
        if (loadLR2others(t))
            return 6;
        if (loadLR2src(t))
            return 7;
        if (loadLR2dst(t))
            return 8;
        if (loadLR2srcnote(t))
            return 9;
        if (loadLR2dstnote(t))
            return 10;
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

void SkinLR2::loadLR2IF(const Tokens &t, std::ifstream& lr2skin)
{
    bool optSwitch = true;
    if (t[0] != "#ELSE")
    {
        LOG_WARNING << "[Skin] " << line << ": No IF parameters " << " (Line " << line << ")";
    }
    for (auto it = ++t.begin(); it != t.end(); ++it)
    {
        auto opt = stoub(*it);
        if (opt.first == -1)
        {
            LOG_WARNING << "[Skin] " << line << ": Invalid DST_OPTION Index, deal as false (Line " << line << ")";
            optSwitch = false;
            break;
        }
        bool dstoption = getDstOpt((dst_option)opt.first);
        if (opt.second) dstoption = !dstoption;
        optSwitch = optSwitch && dstoption;
    }

    if (optSwitch)
    {
        while (!lr2skin.eof())
        {
            auto tokens = csvNextLineTokenize(lr2skin);
            if (tokens.begin() == tokens.end()) continue;

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
                loadLR2SkinLine(tokens);
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
                loadLR2IF(tokens, lr2skin);
                return;
            }
            else if (*tokens.begin() == "#ELSEIF")
            {
                loadLR2IF(tokens, lr2skin);
                return;
            }
            else if (*tokens.begin() == "#ENDIF")
                return;
        }
    }
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////

int SkinLR2::loadLR2header(const Tokens &t)
{
    if (t[0] == "#INFORMATION")
    {
        int type = stoine(t[1]);
        StringContent title = t[2];
        StringContent maker = t[3];
        Path thumbnail(t[4]);

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

    else if (t[0] == "#CUSTOMOPTION")
    {
        StringContent title = t[1];
        int dst_op = stoine(t[2]);
        if (dst_op < 900 || dst_op > 999)
        {
            LOG_WARNING << "[Skin] " << line << ": Invalid option value: " << dst_op << " (Line " << line << ")";
            return -2;
        }
        Tokens op_label;
        for (size_t idx = 3; idx < t.size() && !t[idx].empty(); ++idx)
            op_label.push_back(t[idx]);

        LOG_DEBUG << "[Skin] " << line << ": Loaded Custom option " << title << ": " << dst_op;
        customize.push_back({ (unsigned)dst_op, title, std::move(op_label), 0 });
        return 2;
    }

    else if (t[0] == "#CUSTOMFILE")
    {
        StringContent title = t[1];
        StringContent p = t[2];
        Path pathf(p);
        Path def(t[3]);

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

    else if (t[0] == "#ENDOFHEADER")
    {
        return -1;
    }

    return 0;
}

#pragma endregion

SkinLR2::SkinLR2(Path p)
{
	_laneSprites.resize(channelToIdx(NoteChannelCategory::_, NoteChannelIndex::_));
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
        auto tokens = csvNextLineTokenize(lr2skin);
        if (tokens.begin() == tokens.end()) continue;

        if (loadLR2header(tokens) == -1)
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
            loadLR2IF(tokens, lr2skin);
        else
            loadLR2SkinLine(tokens);
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

////////////////////////////////////////////////////////////////////////////////
constexpr bool dst(eOption option_entry, std::initializer_list<unsigned> entries)
{
    auto op = gOptions.get(option_entry);
    for (auto e : entries)
        if (op == e) return true;
    return false;
}
constexpr bool dst(eOption option_entry, unsigned entry)
{
    return dst(option_entry, { entry });
}

constexpr bool sw(std::initializer_list<eSwitch> entries)
{
    for (auto e : entries)
        if (gSwitches.get(e)) return true;
    return false;
}
constexpr bool sw(eSwitch entry)
{
    return sw({ entry });
}

bool SkinLR2::getDstOpt(dst_option d)
{
    if (d == DST_TRUE) return true;
    if (d == DST_FALSE) return false;
    if ((unsigned)d >= 900)
    {
        if ((unsigned)d > 999)
            return false;
        else
            return customizeDst[(size_t)d - 900];
    }

    namespace o = Option;
    using eo = eOption;
    auto &go = gOptions;

    using es = eSwitch;
    auto &gs = gSwitches;

    switch (d)
    {
        // song select
        case SELECT_SELECTING_FOLDER:
            return o::ENTRY_FOLDER == go.get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_SONG:
            return o::ENTRY_SONG == go.get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_COURSE:
            return o::ENTRY_COURSE == go.get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_NEW_COURSE:
            return o::ENTRY_NEW_COURSE == go.get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_PLAYABLE:
            return dst(eo::SELECT_ENTRY_TYPE, { o::ENTRY_SONG, o::ENTRY_COURSE });

        // mode
        case PLAY_DOUBLE:
            return o::PLAY_DOUBLE == go.get(eo::PLAY_MODE);
        case PLAY_BATTLE:
            return o::PLAY_BATTLE == go.get(eo::PLAY_MODE);
        case DOUBLE_OR_BATTLE:
            return dst(eo::PLAY_MODE, { o::PLAY_DOUBLE, o::PLAY_BATTLE });
        case GHOST_OR_BATTLE:
            //return o::PLAY_DOUBLE == go.get(eo::PLAY_MODE);
            break;

        case NO_PANEL:
            return !sw({
                es::SELECT_PANEL1,
                es::SELECT_PANEL2,
                es::SELECT_PANEL3,
                es::SELECT_PANEL4,
                es::SELECT_PANEL5,
                es::SELECT_PANEL6,
                es::SELECT_PANEL7,
                es::SELECT_PANEL8,
                es::SELECT_PANEL9,
                });
        case PANEL1:
        case PANEL2:
        case PANEL3:
        case PANEL4:
        case PANEL5:
        case PANEL6:
        case PANEL7:
        case PANEL8:
        case PANEL9:
			return sw((es)(d - PANEL1 + (int)es::SELECT_PANEL1));

        // system settings
        case SYSTEM_BGA_NORMAL:
            return dst(eo::PLAY_BGA_TYPE, o::BGA_NORMAL);
        case SYSTEM_BGA_EXTEND:
            return dst(eo::PLAY_BGA_TYPE, o::BGA_EXTEND);
        case SYSTEM_AUTOPLAY_OFF:
            return !sw(es::SYSTEM_AUTOPLAY);
        case SYSTEM_AUTOPLAY_ON:
            return sw(es::SYSTEM_AUTOPLAY);

        case SYSTEM_GHOST_OFF:
            return o::GHOST_OFF == go.get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_A:
            return o::GHOST_TOP == go.get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_B:
            return o::GHOST_SIDE == go.get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_C:
            return o::GHOST_SIDE_BOTTOM == go.get(eo::PLAY_GHOST_TYPE);

        case SYSTEM_SCOREGRAPH_OFF:
            return true;
        case SYSTEM_SCOREGRAPH_ON:
            return false;
        case SYSTEM_BGA_OFF:
            return !sw(es::SYSTEM_BGA);
        case SYSTEM_BGA_ON:
            return sw(es::SYSTEM_BGA);

        case NORMAL_GAUGE_1P:
            return dst(eo::PLAY_GAUGE_TYPE, { o::GAUGE_ASSIST, o::GAUGE_EASY, o::GAUGE_NORMAL });
        case HARD_GAUGE_1P:
            return dst(eo::PLAY_GAUGE_TYPE, { o::GAUGE_HARD, o::GAUGE_EXHARD, o::GAUGE_DEATH });

        case NORMAL_GAUGE_2P:
        case HARD_GAUGE_2P:

        case DIFF_FILTER_ON:
            break;
        case DIFF_FILTER_OFF:
            return true;

#pragma region info
        case OFFLINE:
			return !sw(es::NETWORK);
        case ONLINE:
			return sw(es::NETWORK);

        case EXTRA_MODE_OFF:
			return !sw(es::PLAY_OPTION_EXTRA);
        case EXTRA_MODE_ON:
			return sw(es::PLAY_OPTION_EXTRA);

        case AUTO_SCRATCH_1P_OFF:
        case AUTO_SCRATCH_1P_ON:
        case AUTO_SCRATCH_2P_OFF:
        case AUTO_SCRATCH_2P_ON:

        case SCORE_NOSAVE:
        case SCORE_SAVE:
        case CLEAR_NOSAVE:
        case CLEAR_SAVE_EASY:
        case CLEAR_SAVE_NORMAL:
        case CLEAR_SAVE_HARD:
        case CLEAR_FULLCOMBO_ONLY:

        case LEVEL_INRANGE_BEGINNER:
        case LEVEL_INRANGE_NORMAL:
        case LEVEL_INRANGE_HYPER:
        case LEVEL_INRANGE_ANOTHER:
        case LEVEL_INRANGE_INSANE:
        case LEVEL_OVERFLOW_BEGINNER:
        case LEVEL_OVERFLOW_NORMAL:
        case LEVEL_OVERFLOW_HYPER:
        case LEVEL_OVERFLOW_ANOTHER:
        case LEVEL_OVERFLOW_INSANE:
			break;

        case LOADING:
			return dst(eo::PLAY_SCENE_STAT, o::e_play_scene_stat::LOADING);
        case LOAD_FINISHED:
			return !dst(eo::PLAY_SCENE_STAT, o::e_play_scene_stat::LOADING);

        case REPLAY_OFF:
        case REPLAY_RECORDING:
        case REPLAY_PLAYING:

        case RESULT_CLEAR:
        case RESULT_FAILED:
            break;
#pragma endregion

        // song list status
        case CHART_NOPLAY:
            return o::LAMP_NOPLAY == go.get(eo::SELECT_ENTRY_LAMP);
        case CHART_FAILED:
            return o::LAMP_FAILED == go.get(eo::SELECT_ENTRY_LAMP);
        case CHART_EASY:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_EASY, o::LAMP_ASSIST });
        case CHART_NORMAL:
            return o::LAMP_NORMAL == go.get(eo::SELECT_ENTRY_LAMP);
        case CHART_HARD:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_HARD, o::LAMP_EXHARD });
        case CHART_FULLCOMBO:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_FULLCOMBO, o::LAMP_PERFECT, o::LAMP_MAX });

        case CHART_AAA:
            return dst(eo::SELECT_ENTRY_RANK, { o::RANK_1, o::RANK_0 });
        case CHART_AA:
        case CHART_A:
        case CHART_B:
        case CHART_C:
        case CHART_D:
        case CHART_E:
        case CHART_F:
            return dst(eo::SELECT_ENTRY_RANK, (d - CHART_AA + o::RANK_2));

#pragma region Clear option flag
        case CHART_CLEARED_GAUGE_NORMAL:
        case CHART_CLEARED_GAUGE_HARD:
        case CHART_CLEARED_GAUGE_DEATH:
        case CHART_CLEARED_GAUGE_EASY:
        case CHART_CLEARED_GAUGE_PATTACK:
        case CHART_CLEARED_GAUGE_GATTACK:

        case CHART_CLEARED_SHUFFLE_OFF:
        case CHART_CLEARED_SHUFFLE_MIRROR:
        case CHART_CLEARED_SHUFFLE_RANDOM:
        case CHART_CLEARED_SHUFFLE_SRANDOM:
        case CHART_CLEARED_SHUFFLE_HRANDOM:
        case CHART_CLEARED_SHUFFLE_ALLSCR:

        case CHART_CLEARED_EFFECT_OFF:
        case CHART_CLEARED_EFFECT_HIDDEN:
        case CHART_CLEARED_EFFECT_SUDDEN:
        case CHART_CLEARED_EFFECT_HIDSUD:

        case CHART_CLEARED_ETC_AUTOSCR:
        case CHART_CLEARED_ETC_EXTRAMODE:
        case CHART_CLEARED_ETC_DOUBLEBATTLE:
        case CHART_CLEARED_ETC_SPTODP:
            break;
#pragma endregion

        // metadata
        case CHART_DIFF_NONSET:
        case CHART_DIFF_EASY:
        case CHART_DIFF_NORMAL:
        case CHART_DIFF_HYPER:
        case CHART_DIFF_ANOTHER:
        case CHART_DIFF_INSANE:
            return dst(eo::CHART_DIFFICULTY, d - CHART_DIFF_NONSET + o::DIFF_5);

        case CHART_MODE_7KEYS:
            return dst(eo::CHART_PLAY_KEYS, o::KEYS_7);
        case CHART_MODE_5KEYS:
            return dst(eo::CHART_PLAY_KEYS, o::KEYS_5);
        case CHART_MODE_14KEYS:
            return dst(eo::CHART_PLAY_KEYS, o::KEYS_14);
        case CHART_MODE_10KEYS:
            return dst(eo::CHART_PLAY_KEYS, o::KEYS_10);
        case CHART_MODE_9KEYS:
            return dst(eo::CHART_PLAY_KEYS, o::KEYS_9);

        // play mode after applying mods
        case CHART_PLAYMODE_7KEYS:
            return true;
        case CHART_PLAYMODE_5KEYS:
        case CHART_PLAYMODE_14KEYS:
        case CHART_PLAYMODE_10KEYS:
        case CHART_PLAYMODE_9KEYS:
            break;

        case CHART_NO_BGA:
            return !sw(es::CHART_HAVE_BGA);
        case CHART_HAVE_BGA:
            return sw(es::CHART_HAVE_BGA);
        case CHART_NO_LN:
            return !sw(es::CHART_HAVE_LN);
        case CHART_HAVE_LN:
            return sw(es::CHART_HAVE_LN);
        case CHART_NO_README:
            return !sw(es::CHART_HAVE_README);
        case CHART_HAVE_README:
            return sw(es::CHART_HAVE_README);
        case CHART_NO_BPMCHANGE:
            return !sw(es::CHART_HAVE_BPMCHANGE);
        case CHART_HAVE_BPMCHANGE:
            return sw(es::CHART_HAVE_BPMCHANGE);
        case CHART_NO_RANDOM:
            return !sw(es::CHART_HAVE_RANDOM);
        case CHART_HAVE_RANDOM:
            return sw(es::CHART_HAVE_RANDOM);

        case CHART_JUDGE_VHARD:
        case CHART_JUDGE_HARD:
        case CHART_JUDGE_NORMAL:
        case CHART_JUDGE_EASY:
            return dst(eo::CHART_JUDGE_TYPE, d - CHART_JUDGE_VHARD + o::JUDGE_EASY);

        case CHART_LEVEL_INRANGE:
        case CHART_LEVEL_OVERFLOW:
            break;

        case CHART_NO_STAGEFILE:
            return !sw(es::CHART_HAVE_STAGEFILE);
        case CHART_HAVE_STAGEFILE:
            return sw(es::CHART_HAVE_STAGEFILE);
        case CHART_NO_BANNER:
            return !sw(es::CHART_HAVE_BANNER);
        case CHART_HAVE_BANNER:
            return sw(es::CHART_HAVE_BANNER);
        case CHART_NO_BACKBMP:
            return !sw(es::CHART_HAVE_BACKBMP);
        case CHART_HAVE_BACKBMP:
            return sw(es::CHART_HAVE_BACKBMP);

        case CHART_NO_REPLAY:
        case CHART_HAVE_REPLAY:
            break;

        // Playing
        case PLAY_AAA_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, { o::RANK_1, o::RANK_0 });
        case PLAY_AA_1P:
        case PLAY_A_1P:
        case PLAY_B_1P:
        case PLAY_C_1P:
        case PLAY_D_1P:
        case PLAY_E_1P:
        case PLAY_F_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, d - PLAY_AA_1P + o::RANK_2);

        case PLAY_AAA_2P:
        case PLAY_AA_2P:
        case PLAY_A_2P:
        case PLAY_B_2P:
        case PLAY_C_2P:
        case PLAY_D_2P:
        case PLAY_E_2P:
        case PLAY_F_2P:
            break;

        case PLAY_ABOVE_AAA:
            return dst(eo::PLAY_RANK_BORDER, { o::RANK_1, o::RANK_0 });
        case PLAY_ABOVE_AA:
        case PLAY_ABOVE_A:
        case PLAY_ABOVE_B:
        case PLAY_ABOVE_C:
        case PLAY_ABOVE_D:
        case PLAY_ABOVE_E:
        case PLAY_ABOVE_F:
            return dst(eo::PLAY_RANK_BORDER, d - PLAY_ABOVE_AA + o::RANK_2);

        case PLAY_0p_TO_10p_1P:
        case PLAY_10p_TO_19p_1P:
        case PLAY_20p_TO_29p_1P:
        case PLAY_30p_TO_39p_1P:
        case PLAY_40p_TO_49p_1P:
        case PLAY_50p_TO_59p_1P:
        case PLAY_60p_TO_69p_1P:
        case PLAY_70p_TO_79p_1P:
        case PLAY_80p_TO_89p_1P:
        case PLAY_90p_TO_99p_1P:
        case PLAY_100p_1P:
            break;

        case PLAY_PERFECT_1P:
        case PLAY_GREAT_1P:
        case PLAY_GOOD_1P:
        case PLAY_BAD_1P:
        case PLAY_POOR_1P:
        case PLAY_BPOOR_1P:
            return dst(eo::PLAY_LAST_JUDGE, d - o::JUDGE_0 + PLAY_PERFECT_1P);

        //公式ハーフスキンの左右のネオン用です 2P側も
        case PLAY_POORBGA_NOT_INTIME_1P:
        case PLAY_POORBGA_INTIME_1P:
            return false;

        case PLAY_0p_TO_10p_2P:
        case PLAY_10p_TO_19p_2P:
        case PLAY_20p_TO_29p_2P:
        case PLAY_30p_TO_39p_2P:
        case PLAY_40p_TO_49p_2P:
        case PLAY_50p_TO_59p_2P:
        case PLAY_60p_TO_69p_2P:
        case PLAY_70p_TO_79p_2P:
        case PLAY_80p_TO_89p_2P:
        case PLAY_90p_TO_99p_2P:
        case PLAY_100p_2P:

        case PLAY_PERFECT_2P:
        case PLAY_GREAT_2P:
        case PLAY_GOOD_2P:
        case PLAY_BAD_2P:
        case PLAY_POOR_2P:
        case PLAY_BPOOR_2P:
            break;

        case PLAY_POORBGA_NOT_INTIME_2P:
        case PLAY_POORBGA_INTIME_2P:
            return false;

        case PLAY_CHANGING_LANECOVER_1P:
        case PLAY_CHANGING_LANECOVER_2P:
            break;

        case PLAY_STAGE1:
        case PLAY_STAGE2:
        case PLAY_STAGE3:
        case PLAY_STAGE4:
            return dst(eo::PLAY_COURSE_STAGE, d - PLAY_STAGE1 + o::STAGE_1);
        //PLAY_STAGE5:
        //PLAY_STAGE6:
        //PLAY_STAGE7:
        //PLAY_STAGE8:
        //PLAY_STAGE9:
        case PLAY_STAGEFINAL:
            return dst(eo::PLAY_COURSE_STAGE, o::STAGE_FINAL);

        case PLAY_MODE_COURSE:
        case PLAY_MODE_NONSTOP:
        case PLAY_MODE_EXHARD:
        case PLAY_MODE_SKILLANALYSIS:
            break;

#pragma region Result
        case RESULT_AAA_1P:
            return dst(eo::RESULT_RANK, { o::RANK_1, o::RANK_0 });
        case RESULT_AA_1P:
        case RESULT_A_1P:
        case RESULT_B_1P:
        case RESULT_C_1P:
        case RESULT_D_1P:
        case RESULT_E_1P:
        case RESULT_F_1P:
            return dst(eo::RESULT_RANK, d - RESULT_AA_1P + o::RANK_2);

        case RESULT_MYBEST_AAA:
            return dst(eo::RESULT_MYBEST_RANK, { o::RANK_1, o::RANK_0 });
        case RESULT_MYBEST_AA:
        case RESULT_MYBEST_A:
        case RESULT_MYBEST_B:
        case RESULT_MYBEST_C:
        case RESULT_MYBEST_D:
        case RESULT_MYBEST_E:
        case RESULT_MYBEST_F:
            return dst(eo::RESULT_MYBEST_RANK, d - RESULT_MYBEST_AA + o::RANK_2);

        case RESULT_スコアが更新された:
        case RESULT_MAXCOMBOが更新された:
        case RESULT_最小BPが更新された:
        case RESULT_トライアルが更新された:
        case RESULT_IRの順位が更新された:
        case RESULT_スコアランクが更新された:

        case RESULT_UPDATE_AAA:
        case RESULT_UPDATE_AA:
        case RESULT_UPDATE_A:
        case RESULT_UPDATE_B:
        case RESULT_UPDATE_C:
        case RESULT_UPDATE_D:
        case RESULT_UPDATE_E:
        case RESULT_UPDATE_F:
            break;

        case RESULT_リザルトフリップ無効:
            return true;
        case RESULT_リザルトフリップ有効:
            return false;

        case RESULT_1PWIN_2PLOSE:
        case RESULT_1PLOSE_2PWIN:
        case RESULT_DRAW:
            break;
#pragma endregion

        // Key config
        case KEYCONFIG_7KEYS:
        case KEYCONFIG_9KEYS:
        case KEYCONFIG_5KEYS:
            break;

#pragma region  select song Others
        case SELECT_NO_BEGINNER_IN_SAME_FOLDER:
        case SELECT_NO_NORMAL_IN_SAME_FOLDER:
        case SELECT_NO_HYPER_IN_SAME_FOLDER:
        case SELECT_NO_ANOTHER_IN_SAME_FOLDER:
        case SELECT_NO_INSANE_IN_SAME_FOLDER:

        case SELECT_HAVE_BEGINNER_IN_SAME_FOLDER:
        case SELECT_HAVE_NORMAL_IN_SAME_FOLDER:
        case SELECT_HAVE_HYPER_IN_SAME_FOLDER:
        case SELECT_HAVE_ANOTHER_IN_SAME_FOLDER:
        case SELECT_HAVE_INSANE_IN_SAME_FOLDER:

        case SELECT_HAVE_ONE_BEGINNER_IN_SAME_FOLDER:
        case SELECT_HAVE_ONE_NORMAL_IN_SAME_FOLDER:
        case SELECT_HAVE_ONE_HYPER_IN_SAME_FOLDER:
        case SELECT_HAVE_ONE_ANOTHER_IN_SAME_FOLDER:
        case SELECT_HAVE_ONE_NSANE_IN_SAME_FOLDER:

        case SELECT_HAVE_COUPLE_BEGINNER_IN_SAME_FOLDER:
        case SELECT_HAVE_COUPLE_NORMAL_IN_SAME_FOLDER:
        case SELECT_HAVE_COUPLE_HYPER_IN_SAME_FOLDER:
        case SELECT_HAVE_COUPLE_ANOTHER_IN_SAME_FOLDER:
        case SELECT_HAVE_COUPLE_NSANE_IN_SAME_FOLDER:

        case SELECT_SCOREBAR_BEGINNER_NO_PLAY:
        case SELECT_SCOREBAR_BEGINNER_FAILED:
        case SELECT_SCOREBAR_BEGINNER_EASY:
        case SELECT_SCOREBAR_BEGINNER_CLEAR:
        case SELECT_SCOREBAR_BEGINNER_HARDCLEAR:
        case SELECT_SCOREBAR_BEGINNER_FULLCOMBO:

        case SELECT_SCOREBAR_NORMAL_NO_PLAY:
        case SELECT_SCOREBAR_NORMAL_FAILED:
        case SELECT_SCOREBAR_NORMAL_EASY:
        case SELECT_SCOREBAR_NORMAL_CLEAR:
        case SELECT_SCOREBAR_NORMAL_HARDCLEAR:
        case SELECT_SCOREBAR_NORMAL_FULLCOMBO:

        case SELECT_SCOREBAR_HYPER_NO_PLAY:
        case SELECT_SCOREBAR_HYPER_FAILED:
        case SELECT_SCOREBAR_HYPER_EASY:
        case SELECT_SCOREBAR_HYPER_CLEAR:
        case SELECT_SCOREBAR_HYPER_HARDCLEAR:
        case SELECT_SCOREBAR_HYPER_FULLCOMBO:

        case SELECT_SCOREBAR_ANOTHER_NO_PLAY:
        case SELECT_SCOREBAR_ANOTHER_FAILED:
        case SELECT_SCOREBAR_ANOTHER_EASY:
        case SELECT_SCOREBAR_ANOTHER_CLEAR:
        case SELECT_SCOREBAR_ANOTHER_HARDCLEAR:
        case SELECT_SCOREBAR_ANOTHER_FULLCOMBO:

        case SELECT_SCOREBAR_INSANE_NO_PLAY:
        case SELECT_SCOREBAR_INSANE_FAILED:
        case SELECT_SCOREBAR_INSANE_EASY:
        case SELECT_SCOREBAR_INSANE_CLEAR:
        case SELECT_SCOREBAR_INSANE_HARDCLEAR:
        case SELECT_SCOREBAR_INSANE_FULLCOMBO:
            break;
#pragma endregion

#pragma region course select
        /////////////////////////////////////
        //　コースセレクト関連

        case MODE_COURSESELECT:
        case MODE_NOT_COURSESELECT:

        case COURSE_STAGE_MORE_THAN_1:
        case COURSE_STAGE_MORE_THAN_2:
        case COURSE_STAGE_MORE_THAN_3:
        case COURSE_STAGE_MORE_THAN_4:
        case COURSE_STAGE_MORE_THAN_5:
        case COURSE_STAGE_MORE_THAN_6:
        case COURSE_STAGE_MORE_THAN_7:
        case COURSE_STAGE_MORE_THAN_8:
        case COURSE_STAGE_MORE_THAN_9:
        case COURSE_STAGE_MORE_THAN_10:

        case COURSESELECT_SELECTING_STAGE1:
        case COURSESELECT_SELECTING_STAGE2:
        case COURSESELECT_SELECTING_STAGE3:
        case COURSESELECT_SELECTING_STAGE4:
        case COURSESELECT_SELECTING_STAGE5:
        case COURSESELECT_SELECTING_STAGE6:
        case COURSESELECT_SELECTING_STAGE7:
        case COURSESELECT_SELECTING_STAGE8:
        case COURSESELECT_SELECTING_STAGE9:
        case COURSESELECT_SELECTING_STAGE10:
            break;
#pragma endregion

#pragma region LR2IR
        case IR_対象ではない:  //???
        case IR_LOADING:
        case IR_LOAD_COMPLETE:
        case IR_NO_PLAYER:
        case IR_FAILED:
        case IR_BANNED_CHART:
        case IR_WAITING_FOR_UPDATE:
        case IR_ACCESS:
        case IR_BUSY:

        case IR_NOT_SHOWING_RANKING:
        case IR_SHOWING_RANKING:

        case IR_NOT_IN_GHOSTBATTLE:
        case IR_IN_GHOSTBATTLE:

        case IR_自分と相手のスコアを比較する状況ではない:    //現状では、ランキング表示中とライバルフォルダ
        case IR_自分と相手のスコアを比較するべき状況である:

        case IR_RIVAL_NOPLAY:
        case IR_RIVAL_FAILED:
        case IR_RIVAL_EASY_CLEARED:
        case IR_RIVAL_NORMAL_CLEARED:
        case IR_RIVAL_HARD_CLEARED:
        case IR_RIVAL_FULL_COMBO:

        case IR_RIVAL_AAA:
        case IR_RIVAL_AA:
        case IR_RIVAL_A:
        case IR_RIVAL_B:
        case IR_RIVAL_C:
        case IR_RIVAL_D:
        case IR_RIVAL_E:
        case IR_RIVAL_F:
#pragma endregion

#pragma region course edit
        //COURSE_STAGE1
        case COURSE_STAGE1_DIFF_NONSET:
        case COURSE_STAGE1_DIFF_BEGINNER:
        case COURSE_STAGE1_DIFF_NORMAL:
        case COURSE_STAGE1_DIFF_HYPER:
        case COURSE_STAGE1_DIFF_ANOTHER:
        case COURSE_STAGE1_DIFF_INSANE:

        //COURSE_STAGE2
        case COURSE_STAGE2_DIFF_NONSET:
        case COURSE_STAGE2_DIFF_BEGINNER:
        case COURSE_STAGE2_DIFF_NORMAL:
        case COURSE_STAGE2_DIFF_HYPER:
        case COURSE_STAGE2_DIFF_ANOTHER:
        case COURSE_STAGE2_DIFF_INSANE:

        //COURSE_STAGE3
        case COURSE_STAGE3_DIFF_NONSET:
        case COURSE_STAGE3_DIFF_BEGINNER:
        case COURSE_STAGE3_DIFF_NORMAL:
        case COURSE_STAGE3_DIFF_HYPER:
        case COURSE_STAGE3_DIFF_ANOTHER:
        case COURSE_STAGE3_DIFF_INSANE:

        //COURSE_STAGE4
        case COURSE_STAGE4_DIFF_NONSET:
        case COURSE_STAGE4_DIFF_BEGINNER:
        case COURSE_STAGE4_DIFF_NORMAL:
        case COURSE_STAGE4_DIFF_HYPER:
        case COURSE_STAGE4_DIFF_ANOTHER:
        case COURSE_STAGE4_DIFF_INSANE:

        //COURSE_STAGE5
        case COURSE_STAGE5_DIFF_NONSET:
        case COURSE_STAGE5_DIFF_BEGINNER:
        case COURSE_STAGE5_DIFF_NORMAL:
        case COURSE_STAGE5_DIFF_HYPER:
        case COURSE_STAGE5_DIFF_ANOTHER:
        case COURSE_STAGE5_DIFF_INSANE:
            break;
#pragma endregion
    }

    return false;
}

void SkinLR2::setCustomDstOpt(unsigned base, unsigned offset, bool val)
{
    if (base + offset < 900 || base + offset > 999) return;
    customizeDst[base + offset - 900] = val;
}

void SkinLR2::clearCustomDstOpt()
{
    memset(customizeDst, 0, sizeof(customizeDst) / sizeof(customizeDst[0]));
}

//////////////////////////////////////////////////

void SkinLR2::update()
{
    // update sprites
    vSkin::update();
    for (auto& e : drawQueue)
    {
        e.draw = getDstOpt(e.op1) && getDstOpt(e.op2) && getDstOpt(e.op3);
    }

    // update what?
    //for (auto& c : _csvIncluded)
    //{
    //    c.update();
    //}
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