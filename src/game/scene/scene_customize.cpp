#include "scene_Customize.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/skin/skin_mgr.h"
#include "game/skin/skin_lr2.h"
#include "game/sound/soundset_lr2.h"

#include "scene_decide.h"

SceneCustomize::SceneCustomize() : vScene(eMode::THEME_SELECT, 240)
{
    _scene = eScene::CUSTOMIZE;
    _updateCallback = std::bind(&SceneCustomize::updateStart, this);

    gCustomizeContext.skinDir = 0;
    gCustomizeContext.optionUpdate = 0;

    if (gInCustomize)
    {
        // topest entry is PLAY7
        selectedMode = eMode::PLAY7;
        gCustomizeContext.mode = selectedMode;
        gNextScene = eScene::PLAY;
        gPlayContext.mode = selectedMode;
        gPlayContext.isAuto = true;
        gCustomizeSceneChanged = true;

        State::set(IndexSwitch::SKINSELECT_7KEYS, true);
        State::set(IndexSwitch::SKINSELECT_5KEYS, false);
        State::set(IndexSwitch::SKINSELECT_14KEYS, false);
        State::set(IndexSwitch::SKINSELECT_10KEYS, false);
        State::set(IndexSwitch::SKINSELECT_9KEYS, false);
        State::set(IndexSwitch::SKINSELECT_SELECT, false);
        State::set(IndexSwitch::SKINSELECT_DECIDE, false);
        State::set(IndexSwitch::SKINSELECT_RESULT, false);
        State::set(IndexSwitch::SKINSELECT_KEYCONFIG, false);
        State::set(IndexSwitch::SKINSELECT_SKINSELECT, false);
        State::set(IndexSwitch::SKINSELECT_SOUNDSET, false);
        State::set(IndexSwitch::SKINSELECT_THEME, false);
        State::set(IndexSwitch::SKINSELECT_7KEYS_BATTLE, false);
        State::set(IndexSwitch::SKINSELECT_5KEYS_BATTLE, false);
        State::set(IndexSwitch::SKINSELECT_9KEYS_BATTLE, false);
        State::set(IndexSwitch::SKINSELECT_COURSE_RESULT, false);
    }
    else
    {
        selectedMode = gCustomizeContext.mode;
    }
    load(selectedMode);

    auto skinFileList = findFiles(utf8_to_utf32(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2Files/Theme/*.lr2skin")), true);
    for (auto& p : skinFileList)
    {
        SkinLR2 s(p, 2);
        skinList[s.info.mode].push_back(fs::absolute(p));
    }

    auto soundsetFileList = findFiles(utf8_to_utf32(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2Files/Sound/*.lr2ss")), true);
    for (auto& p : soundsetFileList)
    {
        soundsetList.push_back(fs::absolute(p));
    }

    SoundMgr::setSysVolume(1.0);

    LOG_DEBUG << "[Customize] Start";

    State::set(IndexTimer::_SCENE_CUSTOMIZE_START, Time().norm());
}

SceneCustomize::~SceneCustomize()
{
    save(selectedMode);

    _input.unregister_p("SCENE_PRESS_CUSTOMIZE");
    _input.loopEnd();
    loopEnd();
}

void SceneCustomize::_updateAsync()
{
    if (!gInCustomize && gNextScene != eScene::SELECT) return;

    if (gAppIsExiting)
    {
        _skin->setHandleMouseEvents(false);
        if (SkinMgr::get(selectedMode))
        {
            SkinMgr::unload(selectedMode);
        }
        gNextScene = eScene::EXIT_TRANS;
        gExitingCustomize = true;
    }

    _updateCallback();
}

void SceneCustomize::updateStart()
{
    Time t;
    Time rt = t - State::get(IndexTimer::_SCENE_CUSTOMIZE_START);
    if (rt.norm() > _skin->info.timeIntro)
    {
        _updateCallback = std::bind(&SceneCustomize::updateMain, this);

        if (gInCustomize)
        {
            using namespace std::placeholders;
            _input.register_p("SCENE_PRESS_CUSTOMIZE", std::bind(&SceneCustomize::inputGamePress, this, _1, _2));
        }

        LOG_DEBUG << "[Customize] State changed to Main";
    }
}

void SceneCustomize::updateMain()
{
    Time t;

    // Mode has changed
    if (gCustomizeContext.mode != selectedMode)
    {
        eMode modeOld = selectedMode;
        selectedMode = gCustomizeContext.mode;
        save(modeOld);

        if (selectedMode == eMode::SOUNDSET)
        {
            _skin->setHandleMouseEvents(false);
            SoundMgr::stopSysSamples();
            load(eMode::SOUNDSET);
            loadLR2Sound();
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
            _skin->setHandleMouseEvents(true);
        }
        else
        {
            _skin->setHandleMouseEvents(false);
            if (!gInCustomize || selectedMode != eMode::MUSIC_SELECT)
            {
                if (SkinMgr::get(selectedMode))
                {
                    SkinMgr::unload(selectedMode);
                }
            }
            load(selectedMode);
            _skin->setHandleMouseEvents(true);

            // reload preview
            if (gInCustomize)
            {
                switch (selectedMode)
                {
                case eMode::PLAY5:
                case eMode::PLAY5_2:
                case eMode::PLAY7:
                case eMode::PLAY7_2:
                case eMode::PLAY9:
                case eMode::PLAY10:
                case eMode::PLAY14:
                    gPlayContext.mode = selectedMode;
                    gPlayContext.isAuto = true;
                    break;
                }
                gNextScene = getSceneFromMode(selectedMode);
                gCustomizeSceneChanged = true;
            }
        }
    }

    // Skin has changed
    if (gCustomizeContext.skinDir != 0)
    {
        if (selectedMode == eMode::SOUNDSET)
        {
            if (soundsetList.size() > 1)
            {
                int selectedIdx;
                for (selectedIdx = 0; selectedIdx < (int)soundsetList.size(); selectedIdx++)
                {
                    Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
                        ConfigMgr::get("S", cfg::S_PATH_SOUNDSET, cfg::S_DEFAULT_PATH_SOUNDSET)));
                    if (fs::exists(soundsetList[selectedIdx]) && fs::exists(path) && fs::equivalent(soundsetList[selectedIdx], path))
                        break;
                }
                selectedIdx += gCustomizeContext.skinDir;
                if (selectedIdx >= (int)soundsetList.size())
                {
                    selectedIdx = 0;
                }
                if (selectedIdx < 0)
                {
                    selectedIdx = (int)soundsetList.size() - 1;
                }
                if (selectedIdx >= 0 && selectedIdx < (int)soundsetList.size())
                {
                    auto& p = fs::relative(soundsetList[selectedIdx], ConfigMgr::get("E", cfg::E_LR2PATH, ".")).string();

                    ConfigMgr::set("S", cfg::S_PATH_SOUNDSET, p);

                    _skin->setHandleMouseEvents(false);
                    SoundMgr::stopSysSamples();
                    load(eMode::SOUNDSET);
                    loadLR2Sound();
                    SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
                    _skin->setHandleMouseEvents(true);
                }
            }
        }
        else if (!gInCustomize && selectedMode == eMode::MUSIC_SELECT)
        {
            // Hold up! You want to change select skin inside select skin?
        }
        else
        {
            if (skinList[selectedMode].size() > 1)
            {
                int selectedIdx;
                auto& currentSkin = SkinMgr::get(selectedMode);
                for (selectedIdx = 0; selectedIdx < (int)skinList[selectedMode].size(); selectedIdx++)
                {
                    const Path& p1 = skinList[selectedMode][selectedIdx];
                    const Path& p2 = currentSkin ? Path(currentSkin->getFilePath()) : Path();
                    if (fs::exists(p1) && fs::exists(p2) && fs::equivalent(p1, p2))
                        break;
                }
                selectedIdx += gCustomizeContext.skinDir;
                if (selectedIdx >= (int)skinList[selectedMode].size())
                {
                    selectedIdx = 0;
                }
                if (selectedIdx < 0)
                {
                    selectedIdx = (int)skinList[selectedMode].size() - 1;
                }
                if (selectedIdx >= 0 && selectedIdx < (int)skinList[selectedMode].size())
                {
                    auto& p = fs::relative(skinList[selectedMode][selectedIdx], ConfigMgr::get("E", cfg::E_LR2PATH, ".")).string();
                    switch (selectedMode)
                    {
                    case eMode::MUSIC_SELECT:
                        ConfigMgr::set("S", cfg::S_PATH_MUSIC_SELECT, p);
                        break;

                    case eMode::DECIDE:
                        ConfigMgr::set("S", cfg::S_PATH_DECIDE, p);
                        break;

                    case eMode::RESULT:
                        ConfigMgr::set("S", cfg::S_PATH_RESULT, p);
                        break;

                    case eMode::KEY_CONFIG:
                        ConfigMgr::set("S", cfg::S_PATH_KEYCONFIG, p);
                        break;

                    case eMode::THEME_SELECT:
                        ConfigMgr::set("S", cfg::S_PATH_CUSTOMIZE, p);
                        break;

                    case eMode::PLAY5:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_5, p);
                        break;

                    case eMode::PLAY5_2:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_5_BATTLE, p);
                        break;

                    case eMode::PLAY7:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_7, p);
                        break;

                    case eMode::PLAY7_2:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_7_BATTLE, p);
                        break;

                    case eMode::PLAY9:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_9, p);
                        break;

                    case eMode::PLAY10:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_10, p);
                        break;

                    case eMode::PLAY14:
                        ConfigMgr::set("S", cfg::S_PATH_PLAY_14, p);
                        break;
                    }

                    _skin->setHandleMouseEvents(false);
                    if (SkinMgr::get(selectedMode))
                    {
                        SkinMgr::unload(selectedMode);
                    }
                    load(selectedMode);
                    _skin->setHandleMouseEvents(true);

                    // reload preview
                    if (gInCustomize)
                    {
                        switch (selectedMode)
                        {
                        case eMode::PLAY5:
                        case eMode::PLAY5_2:
                        case eMode::PLAY7:
                        case eMode::PLAY7_2:
                        case eMode::PLAY9:
                        case eMode::PLAY10:
                        case eMode::PLAY14:
                            gPlayContext.mode = selectedMode;
                            gPlayContext.isAuto = true;
                            break;
                        }
                        gNextScene = getSceneFromMode(selectedMode);
                        gCustomizeSceneChanged = true;
                    }
                }
            }
        }
        gCustomizeContext.skinDir = 0;
    }

    // Option has changed
    if (gCustomizeContext.optionUpdate)
    {
        gCustomizeContext.optionUpdate = false;

        size_t idxOption = topOptionIndex + gCustomizeContext.optionIdx;
        if (idxOption < optionsKeyList.size())
        {
            Option& op = optionsMap[optionsKeyList[idxOption]];
            size_t idxEntry = op.selectedEntry;
            if (gCustomizeContext.optionDir > 0)
            {
                if (idxEntry + 1 >= op.entries.size())
                    idxEntry = 0;
                else
                    idxEntry++;
            }
            else
            {
                if (idxEntry == 0 && gCustomizeContext.optionDir < 0)
                    idxEntry = !op.entries.empty() ? (op.entries.size() - 1) : 0;
                else
                    idxEntry--;
            }
            if (idxEntry != op.selectedEntry)
            {
                setOption(idxOption, idxEntry);
            }

            if (selectedMode == eMode::SOUNDSET)
            {
                SoundMgr::stopSysSamples();
                load(eMode::SOUNDSET);
                loadLR2Sound();
                SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
            }

            // reload preview
            if (gInCustomize)
            {
                switch (selectedMode)
                {
                case eMode::PLAY5:
                case eMode::PLAY5_2:
                case eMode::PLAY7:
                case eMode::PLAY7_2:
                case eMode::PLAY9:
                case eMode::PLAY10:
                case eMode::PLAY14:
                    gPlayContext.mode = selectedMode;
                    gPlayContext.isAuto = true;
                    break;
                }
                gNextScene = getSceneFromMode(selectedMode);
                gCustomizeSceneChanged = true;
            }
        }
    }
    if (gCustomizeContext.optionDragging)
    {
        gCustomizeContext.optionDragging = false;

        topOptionIndex = State::get(IndexSlider::SKIN_CONFIG_OPTIONS) * optionsMap.size();
        updateTexts();
    }
    if (_exiting)
    {
        State::set(IndexTimer::_SCENE_CUSTOMIZE_FADEOUT, t.norm());
        SoundMgr::setSysVolume(0.0, 1000);
        _updateCallback = std::bind(&SceneCustomize::updateFadeout, this);
        using namespace std::placeholders;
        _input.unregister_p("SCENE_PRESS_CUSTOMIZE");
        LOG_DEBUG << "[Customize] State changed to Fadeout";
    }
}

void SceneCustomize::updateFadeout()
{
    Time t;
    Time rt = t - State::get(IndexTimer::_SCENE_CUSTOMIZE_FADEOUT);

    if (rt.norm() > _skin->info.timeOutro)
    {
        _skin->setHandleMouseEvents(false);
        if (SkinMgr::get(selectedMode))
        {
            SkinMgr::unload(selectedMode);
        }
        gNextScene = eScene::SELECT;
        gExitingCustomize = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

StringPath SceneCustomize::getConfigFileName(StringPathView skinPath) 
{
    Path p(skinPath);
    std::string md5 = HashMD5(p.u8string()).hexdigest();
    md5 += ".yaml";
    return Path(md5).native();
}


void SceneCustomize::setOption(size_t idxOption, size_t idxEntry)
{
    switch (_skin->type())
    {
    case eSkinType::LR2:
    {
        assert(idxOption < optionsKeyList.size());
        Option& op = optionsMap[optionsKeyList[idxOption]];
        if (op.id != 0)
        {
            for (size_t i = 0; i < op.entries.size(); ++i)
            {
                setCustomDstOpt(op.id, i, false);
            }
            setCustomDstOpt(op.id, idxEntry, true);
        }
        else
        {
            // save to file when exit
        }
        op.selectedEntry = idxEntry;
        save(selectedMode);
        updateTexts();
        break;
    }

    default:
        break;
    }

    if (selectedMode == eMode::SOUNDSET)
    {
        Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
            ConfigMgr::get("S", cfg::S_PATH_SOUNDSET, cfg::S_DEFAULT_PATH_SOUNDSET)));

        SoundSetLR2 ss(path);
        Path bgmOld = ss.getPathBGMSelect();
        save(eMode::SOUNDSET);
        SoundSetLR2 ss2(path);
        Path bgmNew = ss2.getPathBGMSelect();
        if (bgmOld != bgmNew)
        {
            SoundMgr::stopSysSamples();
            loadLR2Sound();
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_SELECT);
        }
    }
}


void SceneCustomize::load(eMode mode)
{
    StringPath configFilePath;
    if (mode == eMode::SOUNDSET)
    {
        optionsMap.clear();
        optionsKeyList.clear();

        Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
            ConfigMgr::get("S", cfg::S_PATH_SOUNDSET, cfg::S_DEFAULT_PATH_SOUNDSET)));

        SoundSetLR2 ss(path);

        State::set(IndexText::SKIN_NAME, ss.name);
        State::set(IndexText::SKIN_MAKER_NAME, ss.maker);

        // load names
        size_t count = ss.getCustomizeOptionCount();
        for (size_t i = 0; i < count; ++i)
        {
            vSkin::CustomizeOption opSkin = ss.getCustomizeOptionInfo(i);
            Option op;
            op.displayName = opSkin.displayName;
            op.selectedEntry = opSkin.defaultEntry;
            op.id = opSkin.id;
            op.entries = opSkin.entries;
            optionsMap[opSkin.internalName] = op;
            optionsKeyList.push_back(opSkin.internalName);
        }

        configFilePath = ss.getFilePath();
    }
    else
    {
        if (!SkinMgr::get(mode))
            SkinMgr::load(mode, true);
        pSkin ps = SkinMgr::get(mode);
        optionsMap.clear();
        optionsKeyList.clear();

        if (ps != nullptr)
        {
            State::set(IndexText::SKIN_NAME, ps->getName());
            State::set(IndexText::SKIN_MAKER_NAME, ps->getMaker());

            // load names
            size_t count = ps->getCustomizeOptionCount();
            for (size_t i = 0; i < count; ++i)
            {
                vSkin::CustomizeOption opSkin = ps->getCustomizeOptionInfo(i);
                Option op;
                op.displayName = opSkin.displayName;
                op.selectedEntry = opSkin.defaultEntry;
                op.id = opSkin.id;
                op.entries = opSkin.entries;
                optionsMap[opSkin.internalName] = op;
                optionsKeyList.push_back(opSkin.internalName);
            }

            configFilePath = ps->getFilePath();
        }
        else
        {
            State::set(IndexText::SKIN_NAME, "");
            State::set(IndexText::SKIN_MAKER_NAME, "");
        }
    }

    if (!configFilePath.empty())
    {
        // load config from file
        Path pCustomize = ConfigMgr::Profile()->getPath() / "customize" / getConfigFileName(configFilePath);
        try
        {
            for (const auto& node : YAML::LoadFile(pCustomize.u8string()))
            {
                if (auto itOp = optionsMap.find(node.first.as<std::string>()); itOp != optionsMap.end())
                {
                    Option& op = itOp->second;
                    auto selectedEntryName = node.second.as<std::string>();
                    if (const auto itEntry = std::find(op.entries.begin(), op.entries.end(), selectedEntryName); itEntry != op.entries.end())
                    {
                        op.selectedEntry = std::distance(op.entries.begin(), itEntry);
                    }
                }
            }
        }
        catch (YAML::BadFile&)
        {
            LOG_WARNING << "[Customize] Bad file: " << pCustomize.u8string();
        }
    }

    topOptionIndex = 0;
    if (!optionsMap.empty())
    {
        State::set(IndexSlider::SKIN_CONFIG_OPTIONS, double(topOptionIndex) / (optionsMap.size() - 1));
    }
    updateTexts();
}

void SceneCustomize::save(eMode mode) const
{
    Path pCustomize;
    if (mode == eMode::SOUNDSET)
    {
        Path path = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."),
            ConfigMgr::get("S", cfg::S_PATH_SOUNDSET, cfg::S_DEFAULT_PATH_SOUNDSET)));

        SoundSetLR2 ss(path);

        pCustomize = ConfigMgr::Profile()->getPath() / "customize";
        fs::create_directories(pCustomize);
        pCustomize /= getConfigFileName(ss.getFilePath());
    }
    else
    {
        pSkin ps = SkinMgr::get(mode);
        if (ps != nullptr)
        {
            pCustomize = ConfigMgr::Profile()->getPath() / "customize";
            fs::create_directories(pCustomize);
            pCustomize /= getConfigFileName(ps->getFilePath());
        }
    }

    if (!pCustomize.empty())
    {
        YAML::Node yaml;
        for (const auto& itOp : optionsMap)
        {
            auto& [tag, op] = itOp;
            if (!op.entries.empty())
                yaml[tag] = op.entries[op.selectedEntry];
        }

        std::ofstream fout(pCustomize, std::ios_base::trunc);
        fout << yaml;
        fout.close();
    }
}

void SceneCustomize::updateTexts() const
{
    for (size_t i = 0; i < 10; ++i)
    {
        IndexText optionNameId = IndexText(size_t(IndexText::スキンカスタマイズカテゴリ名1個目) + i);
        IndexText entryNameId = IndexText(size_t(IndexText::スキンカスタマイズ項目名1個目) + i);
        size_t idx = topOptionIndex + i;
        if (idx < optionsKeyList.size())
        {
            const Option& op = optionsMap.at(optionsKeyList[idx]);
            State::set(optionNameId, op.displayName);
            State::set(entryNameId, !op.entries.empty() ? op.entries[op.selectedEntry] : "");
        }
        else
        {
            State::set(optionNameId, "");
            State::set(entryNameId, "");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneCustomize::inputGamePress(InputMask& m, const Time& t)
{
    if (m[Input::Pad::ESC]) _exiting = true;
    if (m[Input::Pad::M2]) _exiting = true;

    if (m[Input::Pad::MWHEELUP] && topOptionIndex > 0)
    {
        topOptionIndex--;
        if (!optionsMap.empty())
        {
            State::set(IndexSlider::SKIN_CONFIG_OPTIONS, double(topOptionIndex) / (optionsMap.size() - 1));
        }
        updateTexts();
    }

    if (m[Input::Pad::MWHEELDOWN] && topOptionIndex + 1 < optionsMap.size())
    {
        topOptionIndex++;
        if (!optionsMap.empty())
        {
            State::set(IndexSlider::SKIN_CONFIG_OPTIONS, double(topOptionIndex) / (optionsMap.size() - 1));
        }
        updateTexts();
    }
}

////////////////////////////////////////////////////////////////////////////////

void SceneCustomize::draw() const
{
    // screenshot
    pTexture pTex = _skin->getTextureCustomizeThumbnail();
    graphics_copy_screen_texture(*pTex);

    // draw own things
    vScene::draw();
}