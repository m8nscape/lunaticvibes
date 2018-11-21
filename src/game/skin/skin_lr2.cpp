#include "skin_lr2.h"
#include "logger.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <regex>
#include "game/data/option.h"
#include "game/data/switch.h"

bool SkinLR2::customizeDst[100];

#pragma region LR2 csv parsing

std::vector<StringContent> SkinLR2::csvNextLineTokenize(std::istream& file)
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

    //std::vector<StringContent> ret;
    //auto tokens = tokenizer(line, boost::escaped_list_separator<char>());
    //for (auto& t : tokens) ret.push_back(t);
    //while (!ret.empty() && ret.back().empty()) ret.pop_back();
    //return ret;
    const std::regex re{ R"(((?:[^\\,]|\\.)*?)(?:,|$))" };
    std::vector<StringContent> result = { std::sregex_token_iterator(linecsv.begin(), linecsv.end(), re, 1), std::sregex_token_iterator() };
    size_t lastToken;
    for (lastToken = result.size() - 1; lastToken >= 0 && result[lastToken].empty(); --lastToken);
    result.resize(lastToken + 1);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// File parsing
#pragma region 

int SkinLR2::loadLR2image(const std::vector<StringContent> &t)
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
                LOG_DEBUG << "[Skin] " << line << ": Added random IMAGE[" << imageCount << "]: " << ls[ranidx];
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
            LOG_DEBUG << "[Skin] " << line << ": Added IMAGE[" << imageCount << "]: " << path;
        }
        ++imageCount;
        return 1;
    }
    return 0;
}

int SkinLR2::loadLR2font(const std::vector<StringContent> &t)
{
    // TODO Skipped for now.
    if (t[0] == "#LR2FONT")
    {
        Path path(t[1]);
        //lr2fontPath.push_back(std::move(path));
        LOG_DEBUG << "[Skin] " << line << ": Skipped LR2FONT: " << path;
        return 1;
    }
    return 0;
}

int SkinLR2::loadLR2systemfont(const std::vector<StringContent> &t)
{
    if (t[0] == "#FONT")
    {
        int ptsize = stoine(t[1]);
        int thick = stoine(t[2]);
        int fonttype = stoine(t[3]);
        StringContent name = t[4];
        size_t idx = _fontNameMap.size();
        //_fontNameMap[std::to_string(idx)] = std::make_shared<TTFFont>(path.string().c_str(), ptsize);
        LOG_DEBUG << "[Skin] " << line << ": Skipped FONT[" << idx << "]: " << name;
    }
    return 0;
}

int SkinLR2::loadLR2include(const std::vector<StringContent> &t)
{
    if (t[0] == "#INCLUDE")
    {
        Path path(t[1]);
        auto line = this->line;
        this->line = 0;
        LOG_DEBUG << "[Skin] " << line << ": INCLUDE: " << path;
        auto subCsv = SkinLR2(path);
        if (subCsv._loaded)
            _csvIncluded.push_back(std::move(subCsv));
        LOG_DEBUG << "[Skin] " << line << ": INCLUDE END //" << path;
        this->line = line;
        return 1;
    }
    return 0;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Parameters parsing
#pragma region

int SkinLR2::loadLR2timeoption(const std::vector<StringContent> &t)
{
    if (t[0] == "#STARTINPUT")
    {
        int start = stoine(t[1]);
        if (start > 0) info.timeIntro = start;
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
        if (time > 0) info.timeIntro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set Intro freeze time: " << time;
        return 2;
    }

    else if (t[0] == "#LOADSTART")
    {
        int time = stoine(t[1]);
        if (time > 0) info.timeStartLoading = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown before loading: " << time;
        return 3;
    }

    else if (t[0] == "#LOADEND")
    {
        int time = stoine(t[1]);
        if (time > 0) info.timeMinimumLoad = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time colddown after loading: " << time;
        return 4;
    }

    else if (t[0] == "#PLAYSTART")
    {
        int time = stoine(t[1]);
        if (time > 0) info.timeGetReady = time;
        LOG_DEBUG << "[Skin] " << line << ": Set time READY after loading: " << time;
        return 5;
    }

    else if (t[0] == "#CLOSE")
    {
        int time = stoine(t[1]);
        if (time > 0) info.timeFailed = time;
        LOG_DEBUG << "[Skin] " << line << ": Set FAILED time length: " << time;
        return 6;
    }

    else if (t[0] == "#FADEOUT")
    {
        int time = stoine(t[1]);
        if (time > 0) info.timeOutro = time;
        LOG_DEBUG << "[Skin] " << line << ": Set fadeout time length: " << time;
        return 7;
    }

    return 0;
}

int SkinLR2::loadLR2others(const std::vector<StringContent> &t)
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

int SkinLR2::loadLR2src(const std::vector<StringContent> &t)
{
    auto opt = t[0];

    if (opt == "#SRC_TEXT")
    {
        if (t.size() < 5)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

        int font, st, align, edit, panel;
        font = stoine(t[2]);
        st   = stoine(t[3]);
        align = stoine(t[4]);
        edit  = stoine(t[5]);
        panel = stoine(t[6]);

        //elements.emplace_back();
        //auto& e = elements.back();
        //e = std::make_shared<elemText>();

        // TODO text
        LOG_DEBUG << "[Skin] " << line << ": Skipped Text (font: " << font << ")";

        return 7;
    }

    // skip unsupported
    if (opt != "#SRC_IMAGE" && opt != "#SRC_NUMBER" &&
        opt != "#SRC_SLIDER" && opt != "#SRC_BARGRAPH" &&
        opt != "#SRC_BUTTON" && opt != "#SRC_ONMOUSE")
        return 0;

    if (t.size() < 11)
        LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";

    int gr = stoine(t[2]);
    int x = stoine(t[3]);
    int y = stoine(t[4]);
    int w = stoine(t[5]);
    int h = stoine(t[6]);
    int div_x = stoine(t[7]);
    int div_y = stoine(t[8]);
    int cycle = stoine(t[9]);
    int timer = stoine(t[10]);

    if (div_x == 0)
    {
        LOG_WARNING << "[Skin] " << line << ": div_x is 0 (Line " << line << ")";
        div_x = 1;
    }
    if (div_y == 0)
    {
        LOG_WARNING << "[Skin] " << line << ": div_y is 0 (Line " << line << ")";
        div_y = 1;
    }

    if (x == -1 && y == -1)
    {
        x = y = 0;
    }

    int ret = 0;

    if (opt == "#SRC_IMAGE")
    {
        // TODO convert timer
        eTimer iTimer = (eTimer)timer;
        if (_texNameMap.find(std::to_string(gr)) != _texNameMap.end())
        {
            if (w == -1 && h == -1)
            {
                w = _texNameMap[std::to_string(gr)]->getRect().w;
                h = _texNameMap[std::to_string(gr)]->getRect().h;
            }
            _sprites.push_back(std::make_shared<SpriteAnimated>(_texNameMap[std::to_string(gr)], Rect(x, y, w, h), div_x, div_y, cycle, iTimer));
            LOG_DEBUG << "[Skin] " << line << ": Set Image sprite (texture: " << gr << ", timer: " << timer << ")";
        }
        else
        {
            _sprites.push_back(std::make_shared<SpriteAnimated>(_texNameMap["Error"], Rect( 0, 0, 1, 1 ), div_x, div_y, cycle, iTimer));
            LOG_DEBUG << "[Skin] " << line << ": Set Image sprite (texture: " << gr << "|INVALID, timer: " << timer << ")";
        }
        ret = 1;
    }
    else if (opt == "#SRC_NUMBER")
    {
        int num, align, keta;
        if (t.size() < 14)
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
        num = stoine(t[11]);
        align = stoine(t[12]);
        keta = stoine(t[13]);

        // TODO convert num
        eNumber iNum = (eNumber)num;
        // TODO convert timer
        eTimer iTimer = (eTimer)timer;

        if (_texNameMap.find(std::to_string(gr)) != _texNameMap.end())
        {
            if (w == -1 && h == -1)
            {
                w = _texNameMap[std::to_string(gr)]->getRect().w;
                h = _texNameMap[std::to_string(gr)]->getRect().h;
            }
            _sprites.emplace_back(std::make_shared<SpriteNumber>(_texNameMap[std::to_string(gr)], Rect(x, y, w, h), keta, div_x, div_y, cycle, iNum, iTimer));
            LOG_DEBUG << "[Skin] " << line << ": Set Number sprite (texture: " << gr << ", num: " << (unsigned)iNum << ")";
        }
        else
        {
            _sprites.emplace_back(std::make_shared<SpriteNumber>(_texNameMap["Error"], Rect( 0, 0, 1, 1 ), keta, 1, 1, cycle, iNum, iTimer));
            LOG_DEBUG << "[Skin] " << line << ": Set Number sprite (texture: " << gr << "|INVALID, num: " << (unsigned)iNum << ")";
        }

        ret = 2;
    }
    /*
    else if (opt == "#SRC_SLIDER")
    {
        int muki, range, type, disable;
        if (t.size() < 14)
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            muki = range = type = disable = 0;
        }
        else
        {
            muki = it != t.end() ? stoine(*it++) : 0;
            range = it != t.end() ? stoine(*it++) : 0;
            type = it != t.end() ? stoine(*it++) : 0;
            disable = 0;
            if (it != t.end())
                disable = stoine(*it++);
        }

        e = std::make_shared<elemSlider>(gr, x, y, w, h, div_x, div_y, cycle, timer,
            muki, range, type, disable);

        ret = 3;
    }
    else if (opt == "#SRC_BARGRAPH")
    {
        int type, muki;
        if (t.size() < 13)
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            type = muki = 0;
        }
        else
        {
            type = it != t.end() ? stoine(*it++) : 0;
            muki = it != t.end() ? stoine(*it++) : 0;
        }

        e = std::make_shared<elemBargraph>(gr, x, y, w, h, div_x, div_y, cycle, timer,
            type, muki);

        ret = 4;
    }
    else if (opt == "#SRC_BUTTON")
    {
        int type, click, panel, plusonly;
        if (t.size() < 14)
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            type = click = panel = plusonly = 0;
        }
        else
        {
            type = it != t.end() ? stoine(*it++) : 0;
            click = it != t.end() ? stoine(*it++) : 0;
            panel = it != t.end() ? stoine(*it++) : 0;
            plusonly = 0;
            if (it != t.end())
                plusonly = stoine(*it++);
        }

        e = std::make_shared<elemButton>(gr, x, y, w, h, div_x, div_y, cycle, timer,
            type, click, panel, plusonly);
        ret = 5;
    }
    else if (opt == "#SRC_ONMOUSE")
    {
        int panel, x2, y2, w2, h2;
        if (t.size() < 16)
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            panel = x2 = y2 = w2 = h2 = 0;
        }
        else
        {
            panel = it != t.end() ? stoine(*it++) : 0;
            x2 = it != t.end() ? stoine(*it++) : 0;
            y2 = it != t.end() ? stoine(*it++) : 0;
            w2 = it != t.end() ? stoine(*it++) : 0;
            h2 = it != t.end() ? stoine(*it++) : 0;
        }

        e = std::make_shared<elemOnMouse>(gr, x, y, w, h, div_x, div_y, cycle, timer,
            panel, x2, y2, w2, h2);
        ret = 6;
    }
    */
    return ret;
}

int SkinLR2::loadLR2dst(const std::vector<StringContent> &t)
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

    int time = stoine(t[2]);
    int x = stoine(t[3]);
    int y = stoine(t[4]);
    int w = stoine(t[5]);
    int h = stoine(t[6]);
    int acc = stoine(t[7]);
    int a = stoine(t[8]);
    int r = stoine(t[9]);
    int g = stoine(t[10]);
    int b = stoine(t[11]);
    int blend = stoine(t[12]);
    int filter = stoine(t[13]);
    int angle = stoine(t[14]);
    int center = stoine(t[15]);

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
        int loop, dstTimer, op1, op2, op3, op4;
        if (t[16].empty())
        {
            LOG_WARNING << "[Skin] " << line << ": Parameter not enough (Line " << line << ")";
            loop = -1;
            dstTimer = op1 = op2 = op3 = op4 = 0;
        }
        else
        {
            loop = stoine(t[16]);
            dstTimer = stoine(t[17]);
            op1 = op2 = op3 = op4 = 0;

            {
                StringContent op = t[18];
                if (op[0] == '!' || op[0] == '-')
                    op1 = -stoine(op.substr(1));
                else
                    op1 = stoine(op);
            }
            {
                StringContent op = t[19];
                if (op[0] == '!' || op[0] == '-')
                    op2 = -stoine(op.substr(1));
                else
                    op2 = stoine(op);
            }
            {
                StringContent op = t[20];
                if (op[0] == '!' || op[0] == '-')
                    op3 = -stoine(op.substr(1));
                else
                    op3 = stoine(op);
            }
            {
                StringContent op = t[21];
                if (op[0] == '!' || op[0] == '-')
                    op4 = -stoine(op.substr(1));
                else
                    op4 = stoine(op);
            }
        }

        elements.push_back({e, false, (dst_option)op1, (dst_option)op2, (dst_option)op3, (dst_option)op4});
        e->setLoopTime(loop);
        e->setBlendMode(BlendMode::ALPHA);
        LOG_DEBUG << "[Skin] " << line << ": Set sprite Options (loop: " << loop << ", dst: [" << op1 << "," << op2 << "," << op3 << "]) ";
        if (time > 0)
        {
            LOG_WARNING << "[Skin] " << line << ": First keyframe time is not 0";
            e->appendKeyFrame({ 0, {Rect(x, y, w, h), (RenderParams::accTy)acc, Color(r, g, b, a), (double)angle } });
        }
    }

    e->appendKeyFrame({ time, {Rect(x, y, w, h), (RenderParams::accTy)acc, Color(r, g, b, a), (double)angle } });
    //e->pushKeyFrame(time, x, y, w, h, acc, r, g, b, a, blend, filter, angle, center);
    LOG_DEBUG << "[Skin] " << line << ": Set sprite Keyframe (time: " << time << ")";

    return ret;
}


int SkinLR2::loadLR2note(const std::vector<StringContent> &t)
{
    // TODO create note sprite
    return -1;
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Dispatcher
#pragma region

int SkinLR2::loadLR2SkinLine(const std::vector<StringContent> &raw)
{
    std::vector<StringContent> t;
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
        if (loadLR2note(t))
            return 9;
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

void SkinLR2::loadLR2IF(const std::vector<StringContent> &t, std::ifstream& lr2skin)
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

int SkinLR2::loadLR2header(const std::vector<StringContent> &t)
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
            LOG_WARNING << "[Skin] " << line << ": thumbnail loading failed: " << thumbnail << " (Line " << line << ")";

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
        std::vector<StringContent> op_label;
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

        LOG_DEBUG << "[Skin] " << line << ": Loaded Custom file " << title << ": " << pathf;
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
    std::ifstream lr2skin(p, std::ios::binary);
    if (!lr2skin.is_open())
    {
        LOG_ERROR << "[Skin] " << line << ": Skin File Not Found: " << std::filesystem::absolute(p);
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


    LOG_DEBUG << "[Skin] " << line << ": Loaded " << p;
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
    auto op = gOptions::get(option_entry);
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
        if (gSwitches::get(e)) return true;
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
    typedef gOptions go;

    using es = eSwitch;
    typedef gSwitches gs;

    switch (d)
    {
        // song select
        case SELECT_SELECTING_FOLDER:
            return o::ENTRY_FOLDER == go::get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_SONG:
            return o::ENTRY_SONG == go::get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_COURSE:
            return o::ENTRY_COURSE == go::get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_NEW_COURSE:
            return o::ENTRY_NEW_COURSE == go::get(eo::SELECT_ENTRY_TYPE);
        case SELECT_SELECTING_PLAYABLE:
            return dst(eo::SELECT_ENTRY_TYPE, { o::ENTRY_SONG, o::ENTRY_COURSE });

        // mode
        case PLAY_DOUBLE:
            return o::PLAY_DOUBLE == go::get(eo::PLAY_MODE);
        case PLAY_BATTLE:
            return o::PLAY_BATTLE == go::get(eo::PLAY_MODE);
        case DOUBLE_OR_BATTLE:
            return dst(eo::PLAY_MODE, { o::PLAY_DOUBLE, o::PLAY_BATTLE });
        case GHOST_OR_BATTLE:
            //return o::PLAY_DOUBLE == go::get(eo::PLAY_MODE);
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
            return sw(es::SELECT_PANEL1);
        case PANEL2:
            return sw(es::SELECT_PANEL2);
        case PANEL3:
            return sw(es::SELECT_PANEL3);
        case PANEL4:
            return sw(es::SELECT_PANEL4);
        case PANEL5:
            return sw(es::SELECT_PANEL5);
        case PANEL6:
            return sw(es::SELECT_PANEL6);
        case PANEL7:
            return sw(es::SELECT_PANEL7);
        case PANEL8:
            return sw(es::SELECT_PANEL8);
        case PANEL9:
            return sw(es::SELECT_PANEL9);

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
            return o::GHOST_OFF == go::get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_A:
            return o::GHOST_TOP == go::get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_B:
            return o::GHOST_SIDE == go::get(eo::PLAY_GHOST_TYPE);
        case SYSTEM_GHOST_TYPE_C:
            return o::GHOST_SIDE_BOTTOM == go::get(eo::PLAY_GHOST_TYPE);

        case SYSTEM_SCOREGRAPH_OFF:
            return true;
        case SYSTEM_SCOREGRAPH_ON:
            return false;
        case SYSTEM_BGA_OFF:
            return !sw(es::SYSTEM_BGA);
        case SYSTEM_BGA_ON:
            return !sw(es::SYSTEM_BGA);

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
        case ONLINE:

        case EXTRA_MODE_OFF:
        case EXTRA_MODE_ON:

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

        case LOADING:
        case LOAD_FINISHED:

        case REPLAY_OFF:
        case REPLAY_RECORDING:
        case REPLAY_PLAYING:

        case RESULT_CLEAR:
        case RESULT_FAILED:
            break;
#pragma endregion

        // song list status
        case CHART_NOPLAY:
            return o::LAMP_NOPLAY == go::get(eo::SELECT_ENTRY_LAMP);
        case CHART_FAILED:
            return o::LAMP_FAILED == go::get(eo::SELECT_ENTRY_LAMP);
        case CHART_EASY:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_EASY, o::LAMP_ASSIST });
        case CHART_NORMAL:
            return o::LAMP_NORMAL == go::get(eo::SELECT_ENTRY_LAMP);
        case CHART_HARD:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_HARD, o::LAMP_EXHARD });
        case CHART_FULLCOMBO:
            return dst(eo::SELECT_ENTRY_LAMP, { o::LAMP_FULLCOMBO, o::LAMP_PERFECT, o::LAMP_MAX });

        case CHART_AAA:
            return dst(eo::SELECT_ENTRY_RANK, { o::RANK_1, o::RANK_0 });
        case CHART_AA:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_2);
        case CHART_A:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_3);
        case CHART_B:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_4);
        case CHART_C:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_5);
        case CHART_D:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_6);
        case CHART_E:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_7);
        case CHART_F:
            return dst(eo::SELECT_ENTRY_RANK, o::RANK_8);

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
            return dst(eo::CHART_DIFFICULTY, o::DIFF_0);
        case CHART_DIFF_EASY:
            return dst(eo::CHART_DIFFICULTY, o::DIFF_1);
        case CHART_DIFF_NORMAL:
            return dst(eo::CHART_DIFFICULTY, o::DIFF_2);
        case CHART_DIFF_HYPER:
            return dst(eo::CHART_DIFFICULTY, o::DIFF_3);
        case CHART_DIFF_ANOTHER:
            return dst(eo::CHART_DIFFICULTY, o::DIFF_4);
        case CHART_DIFF_INSANE:
            return dst(eo::CHART_DIFFICULTY, o::DIFF_5);

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
            return dst(eo::CHART_JUDGE_TYPE, o::JUDGE_VHARD);
        case CHART_JUDGE_HARD:
            return dst(eo::CHART_JUDGE_TYPE, o::JUDGE_HARD);
        case CHART_JUDGE_NORMAL:
            return dst(eo::CHART_JUDGE_TYPE, o::JUDGE_NORMAL);
        case CHART_JUDGE_EASY:
            return dst(eo::CHART_JUDGE_TYPE, o::JUDGE_EASY);

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
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_2);
        case PLAY_A_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_3);
        case PLAY_B_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_4);
        case PLAY_C_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_5);
        case PLAY_D_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_6);
        case PLAY_E_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_7);
        case PLAY_F_1P:
            return dst(eo::PLAY_RANK_ESTIMATED, o::RANK_8);

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
            return dst(eo::PLAY_RANK_BORDER, o::RANK_2);
        case PLAY_ABOVE_A:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_3);
        case PLAY_ABOVE_B:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_4);
        case PLAY_ABOVE_C:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_5);
        case PLAY_ABOVE_D:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_6);
        case PLAY_ABOVE_E:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_7);
        case PLAY_ABOVE_F:
            return dst(eo::PLAY_RANK_BORDER, o::RANK_8);

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
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_0);
        case PLAY_GREAT_1P:
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_1);
        case PLAY_GOOD_1P:
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_2);
        case PLAY_BAD_1P:
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_3);
        case PLAY_POOR_1P:
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_4);
        case PLAY_BPOOR_1P:
            return dst(eo::PLAY_LAST_JUDGE, o::JUDGE_5);

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
            return dst(eo::PLAY_COURSE_STAGE, o::STAGE_1);
        case PLAY_STAGE2:
            return dst(eo::PLAY_COURSE_STAGE, o::STAGE_2);
        case PLAY_STAGE3:
            return dst(eo::PLAY_COURSE_STAGE, o::STAGE_3);
        case PLAY_STAGE4:
            return dst(eo::PLAY_COURSE_STAGE, o::STAGE_4);
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
            return dst(eo::RESULT_RANK, o::RANK_2);
        case RESULT_A_1P:
            return dst(eo::RESULT_RANK, o::RANK_3);
        case RESULT_B_1P:
            return dst(eo::RESULT_RANK, o::RANK_4);
        case RESULT_C_1P:
            return dst(eo::RESULT_RANK, o::RANK_5);
        case RESULT_D_1P:
            return dst(eo::RESULT_RANK, o::RANK_6);
        case RESULT_E_1P:
            return dst(eo::RESULT_RANK, o::RANK_7);
        case RESULT_F_1P:
            return dst(eo::RESULT_RANK, o::RANK_8);

        case RESULT_MYBEST_AAA:
            return dst(eo::RESULT_MYBEST_RANK, { o::RANK_1, o::RANK_0 });
        case RESULT_MYBEST_AA:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_2);
        case RESULT_MYBEST_A:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_3);
        case RESULT_MYBEST_B:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_4);
        case RESULT_MYBEST_C:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_5);
        case RESULT_MYBEST_D:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_6);
        case RESULT_MYBEST_E:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_7);
        case RESULT_MYBEST_F:
            return dst(eo::RESULT_MYBEST_RANK, o::RANK_8);

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


////////////////////////////////////////////////////////////////////////////////

void SkinLR2::update()
{
    vSkin::update();
    for (auto& e : elements)
    {
        e.draw = getDstOpt(e.op1) && getDstOpt(e.op2) && getDstOpt(e.op3);
    }
    for (auto& c : _csvIncluded)
    {
        c.update();
    }
}

void SkinLR2::draw() const
{
    for (auto& e : elements)
    {
        if (e.draw) e.ps->draw();
    }
    for (auto& c : _csvIncluded)
    {
        c.draw();
    }
}